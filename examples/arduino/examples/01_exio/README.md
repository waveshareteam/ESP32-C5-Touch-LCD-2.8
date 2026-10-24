# IO Expander

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Toggles CH32V003 header EXIO6/7 and backlight PWM once per second.

## Hardware Required

ESP32-C5-Touch-LCD-2.8 and USB data cable; optionally attach a meter to EXIO6/7.

## How to Run

Set the sketchbook to examples/arduino/ and open this sketch. Use Arduino-ESP32 from [product.json](../../../../.github/product.json): ESP32C5 Dev Module, 16 MB Flash, QIO, PSRAM enabled and 16M Flash (3MB APP/9.9MB FATFS), then upload.

## Expected Result

Serial output reports EXIO6/7 level and backlight percentage, alternating once per second.

## Troubleshooting

Keep CH32V003 firmware. Reset, amplifier and SHTC3 pins are not test outputs; see the [pin-purpose guide](../../../../docs/ch32v003.md). The LCD is not initialized in this test.
