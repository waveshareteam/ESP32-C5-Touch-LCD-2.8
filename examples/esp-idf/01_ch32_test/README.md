# CH32V003 IO Test

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Initialize the IO expander and exercise its inputs and outputs.

## Hardware Required

ESP32-C5-Touch-LCD-2.8. The board and a USB cable.

## How to Run

From this directory, activate a supported ESP-IDF environment and run:

```sh
idf.py set-target esp32c5 build flash monitor
```

## Expected Result

Serial output reports IO operations and input levels.

## Troubleshooting

The CH32V003 must retain its factory IO-expander firmware.
