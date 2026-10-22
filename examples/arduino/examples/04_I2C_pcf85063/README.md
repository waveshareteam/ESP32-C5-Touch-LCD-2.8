# PCF85063 RTC

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Initialize the RTC and read its calendar/time.

## Hardware Required

ESP32-C5-Touch-LCD-2.8. The board and a USB cable.

## How to Run

Install the Arduino-ESP32 version in [product.json](../../../../.github/product.json), set the sketchbook to examples/arduino/, then open this sketch and upload. Select ESP32C5 Dev Module, 16 MB Flash, QIO, PSRAM enabled and 16M Flash (3MB APP/9.9MB FATFS).

## Expected Result

Serial output shows the RTC time.

## Troubleshooting

Check the example's initial calendar before using it as a clock.
