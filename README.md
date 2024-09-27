# GStreamer DJI Moonlight Shim Plugin

Stream games, videos, or any content via GStreamer directly to your DJI FPV Goggles!

The DJI Moonlight project enables a versatile and immersive experience by allowing you to stream a variety of content to your DJI FPV Goggles over USB. Whether it's gaming, video, or other real-time streams like those from OpenIPC, this setup transforms your DJI FPV Goggles into a high-quality display for virtually any source.

The `libgstdjimoonlightshim.so` plugin provides an interface for integrating DJI Moonlight systems into GStreamer pipelines. It enables streaming or handling media directly through GStreamer with DJI hardware.

Note: To use this plugin, you must install [dji-moonlight-shim](https://github.com/fpv-wtf/dji-moonlight-shim) on your DJI FPV Goggles. Follow the installation instructions for dji-moonlight-shim before proceeding with the steps below.

## Requirements
Before building and using the `djimoonlightshim` GStreamer plugin, ensure you have the following prerequisites installed:
```bash
# On Ubuntu/Debian:
sudo apt install cmake
sudo apt install libgstreamer1.0-dev gstreamer1.0-plugins-base gstreamer1.0-tools gstreamer1.0-plugins-good gstreamer1.0-plugins-bad gstreamer1.0-plugins-ugly 
sudo apt install libusb-1.0-0-dev
```

## Build the Plugin

Create a build directory, configure the project using CMake, and then build the plugin:
```bash
mkdir build
cd build
cmake ../
make
```

## Install the Plugin

Copy the shared object file (.so) to the GStreamer plugin directory:
```bash
sudo cp libgstdjimoonlightshim.so /usr/lib/gstreamer-1.0/
```


## Verify the Installation

After copying the plugin, ensure GStreamer recognizes it by running:
```bash
gst-inspect-1.0 djimoonlightshim
```
If the plugin is successfully loaded, you will see detailed information about the djimoonlightshim plugin.


## Usage
Once installed, you can use the djimoonlightshim plugin within GStreamer pipelines. Below is an example pipeline using videotestsrc:
```bash
gst-launch-1.0 videotestsrc ! "video/x-raw,width=1920,height=1080,framerate=30/1" ! x264enc ! h264parse ! djimoonlightshimsink
```


Thanks to the [fpv.wtf team](https://github.com/fpv-wtf) team for providing the shim, enabling root access, and creating the customizations that allow custom software to run on the DJI FPV Goggles.
