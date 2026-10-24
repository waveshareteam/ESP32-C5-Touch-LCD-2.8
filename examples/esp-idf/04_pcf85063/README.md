# PCF85063 RTC

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Sets a demonstration date and reads the hardware RTC.

## Hardware Required

Board and USB data cable.

## How to Run

Activate an ESP-IDF version from [product.json](../../../.github/product.json), then run `idf.py set-target esp32c5`, `idf.py build` and `idf.py -p PORT flash monitor` in this directory. Keep the repository layout so the shared components can be found.

## Expected Result

The serial log prints the calendar once per second.

## Troubleshooting

The demonstration date is written on every boot; remove that setter to preserve an existing RTC calendar.
