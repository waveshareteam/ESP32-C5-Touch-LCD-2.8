# SHTC3 Environment

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Reads ambient temperature and relative humidity without starting the display.

## Hardware Required

Board and USB data cable.

## How to Run

Activate an ESP-IDF version from [product.json](../../../.github/product.json), then run `idf.py set-target esp32c5`, `idf.py build` and `idf.py -p PORT flash monitor` in this directory. Keep the repository layout so the shared components can be found.

## Expected Result

The serial log updates Celsius and %RH once per second.

## Troubleshooting

Keep the onboard CH32V003 firmware; EXIO4/5 are used by the sensor and must not be toggled.
