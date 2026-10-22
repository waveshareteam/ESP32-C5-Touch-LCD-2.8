# LVGL Display

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Test LCD, touch input and backlight with the bundled LVGL 8.4.

## Hardware Required

ESP32-C5-Touch-LCD-2.8. The board and a USB cable.

## How to Run

Install the Arduino-ESP32 version in [product.json](../../../../.github/product.json), set the sketchbook to examples/arduino/, then open this sketch and upload. Select ESP32C5 Dev Module, 16 MB Flash, QIO, PSRAM enabled and 16M Flash (3MB APP/9.9MB FATFS).

## Expected Result

The interface updates touch information and supports backlight adjustment.

## Troubleshooting

Use the bundled LVGL version and libraries/lv_conf.h; do not mix LVGL major versions.
