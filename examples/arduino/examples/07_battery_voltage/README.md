# Battery Voltage

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Displays battery voltage and the raw CH32V003 ADC value on LCD and serial output, using an eight-sample average.

## Hardware Required

ESP32-C5-Touch-LCD-2.8, USB data cable and a battery connected to the battery socket.

## How to Run

Set the sketchbook to examples/arduino/ and open this sketch. Use Arduino-ESP32 from [product.json](../../../../.github/product.json): ESP32C5 Dev Module, 16 MB Flash, QIO, PSRAM enabled and 16M Flash (3MB APP/9.9MB FATFS), then upload.

## Expected Result

Battery voltage in volts and ADC (0–1023) update about once per second.

## Troubleshooting

The formula uses nominal 3.3 V reference and a 3:1 divider; it does not estimate charge percentage. Keep the bundled LVGL 8.4 configuration; see the [ADC guide](../../../../docs/ch32v003.md).
