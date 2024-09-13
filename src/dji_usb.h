#ifndef DJI_USB_H
#define DJI_USB_H

#include <stdint.h>
#include <stdbool.h>
#include <libusb-1.0/libusb.h>

/**
 * @brief Initialize the USB BULK device
 *
 * @return true
 * @return false
 */
bool dji_usb_setup();

/**
 * @brief Start stream by sending header with magic word and info to configure decoder
 *
 * @param width
 * @param height
 * @param framerate
 * @return true
 * @return false
 */
bool dji_usb_connect(int width, int height, int framerate);

/**
 * @brief Send H264 encoded frame
 *
 * @param buf buffer of frame
 * @param len length of buffer
 * @return true
 * @return false
 */
bool dji_usb_transmit(uint8_t *buf, uint32_t len);

/**
 * @brief Stop stream without closing device
 *
 * @return true
 * @return false
 */
bool dji_usb_reset();

/**
 * @brief Close USB BULK device
 *
 */
void dji_usb_cleanup();

#endif
