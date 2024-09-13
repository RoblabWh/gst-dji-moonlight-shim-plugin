#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "dji_usb.h"

#define USB_VID 0x2ca3
#define USB_PID 0x1f
#define USB_CONFIG 1
#define USB_INTERFACE 3
#define USB_ENDPOINT_OUT 0x03
#define DJI_HEADER_MAGIC 0x00042069

typedef struct connect_header_s
{
    uint32_t magic;
    uint32_t width;
    uint32_t height;
    uint32_t framerate;
} connect_header_t;

libusb_context *ctx = NULL;
libusb_device_handle *dev = NULL;

bool dji_usb_setup()
{
    int r = libusb_init(&ctx);
    if (r < 0)
    {
        fprintf(stderr, "unable to init libusb: %s\n", libusb_strerror(r));
        return false;
    }

    dev = libusb_open_device_with_vid_pid(ctx, USB_VID, USB_PID);
    if (dev == NULL)
    {
        libusb_exit(ctx);
        fprintf(stderr, "unable to open device, or device not found\n");
        if (geteuid() != 0)
        {
            fprintf(stderr, "try running as root (with sudo)\n");
        }
        return false;
    }

    r = libusb_claim_interface(dev, USB_INTERFACE);
    if (r != 0)
    {
        fprintf(stderr, "unable to claim interface: (%d) %s\n", r, libusb_strerror(r));
        return false;
    }

    return true;
}

bool dji_usb_connect(int width, int height, int framerate)
{
    connect_header_t header;
    header.magic = DJI_HEADER_MAGIC;
    header.width = width;
    header.height = height;
    header.framerate = framerate;

    int r = libusb_bulk_transfer(dev, USB_ENDPOINT_OUT, (unsigned char *)&header, sizeof(header), NULL, 0);
    if (r != 0)
    {
        fprintf(stderr, "unable to send header: (%d) %s\n", r, libusb_strerror(r));
        return false;
    }

    return true;
}

void dji_usb_cleanup()
{
    libusb_release_interface(dev, USB_INTERFACE);
    libusb_close(dev);
    libusb_exit(ctx);
}

bool dji_usb_transmit(uint8_t *buf, uint32_t len)
{
    int ret;
    ret = libusb_bulk_transfer(dev, USB_ENDPOINT_OUT, (uint8_t *)&len, sizeof(len), NULL, 0);
    if (ret)
        return ret;
    ret = libusb_bulk_transfer(dev, USB_ENDPOINT_OUT, buf, len, NULL, 0);
    return !ret;
}

bool dji_usb_reset()
{
    return !libusb_bulk_transfer(dev, USB_ENDPOINT_OUT, NULL, 0, NULL, 0);
}
