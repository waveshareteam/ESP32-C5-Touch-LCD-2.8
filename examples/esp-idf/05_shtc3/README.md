# SHTC3 Environment

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Reads ambient temperature and relative humidity without starting the display.

## Hardware Required

Board and USB data cable.

## How to Run

ESP32-C5 is selected by default. Activate ESP-IDF v5.5.4, then run `idf.py build` and `idf.py -p PORT flash monitor` in this directory. This project includes its own components and can be copied and built independently.

## Expected Result

The serial log updates Celsius and %RH once per second.

## Troubleshooting

Keep the onboard CH32V003 firmware; EXIO4/5 are used by the sensor and must not be toggled.
