# Battery Voltage

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Averages eight CH32V003 ADC samples and displays nominal battery voltage and the raw value.

## Hardware Required

Board, USB cable and battery connected to the battery socket.

## How to Run

ESP32-C5 is selected by default. Activate ESP-IDF v5.5.4, then run `idf.py build` and `idf.py -p PORT flash monitor` in this directory. This project includes its own components and can be copied and built independently.

## Expected Result

LCD and serial output update voltage and ADC (0–1023) about once per second.

## Troubleshooting

Uses a nominal 3.3 V ADC reference and the schematic's 3:1 divider. It does not estimate charge percentage.
