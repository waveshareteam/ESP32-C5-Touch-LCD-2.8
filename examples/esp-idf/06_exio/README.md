# CH32V003 IO

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Toggles header EXIO6/7 and the backlight PWM once per second.

## Hardware Required

Board and USB cable; optionally connect a meter to EXIO6/7.

## How to Run

Activate an ESP-IDF version from [product.json](../../../.github/product.json), then run `idf.py set-target esp32c5`, `idf.py build` and `idf.py -p PORT flash monitor` in this directory. Keep the repository layout so the shared components can be found.

## Expected Result

EXIO6/7 alternate high/low; the log reports output level and PWM value.

## Troubleshooting

The LCD is not initialized. Only EXIO6/7 are test outputs; see the [pin-purpose guide](../../../docs/ch32v003.md).
