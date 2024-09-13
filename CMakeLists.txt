cmake_minimum_required(VERSION 3.15)
project(gst-dji-moonlight-shim-plugin VERSION 1.20.3.0 LANGUAGES C)

set(GST_API_VERSION 1.0)
set(GST_LICENSE LGPL)
set(GST_PACKAGE_NAME "Gstreamer DJI Moonlight Shim Plugin")
set(GST_PACKAGE_ORIGIN "https://w-hs.de")
set(PACKAGE "${PROJECT_NAME}")
set(PACKAGE_VERSION "${PROJECT_VERSION}")

find_package(PkgConfig REQUIRED)
pkg_search_module(gstreamer-base REQUIRED IMPORTED_TARGET gstreamer-base-1.0 gstreamer-base)
pkg_search_module(libusb REQUIRED IMPORTED_TARGET libusb-1.0 libusb)

configure_file(config.h.in config.h @ONLY)

add_library(gstdjimoonlightshim SHARED src/gstdjimoonlightshimsink.c src/dji_usb.c)
target_link_libraries(gstdjimoonlightshim PUBLIC PkgConfig::gstreamer-base PkgConfig::libusb)
target_include_directories(gstdjimoonlightshim PRIVATE ${PROJECT_BINARY_DIR})
target_compile_options(gstdjimoonlightshim PRIVATE -Wall)

install(TARGETS gstdjimoonlightshim DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/${CMAKE_SYSTEM_PROCESSOR}-linux-gnu/gstreamer-1.0/)
