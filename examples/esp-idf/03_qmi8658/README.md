# QMI8658 IMU

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Reads acceleration and angular velocity through the shared BSP.

## Hardware Required

Board and USB data cable.

## How to Run

Activate an ESP-IDF version from [product.json](../../../.github/product.json), then run `idf.py set-target esp32c5`, `idf.py build` and `idf.py -p PORT flash monitor` in this directory. Keep the repository layout so the shared components can be found.

## Expected Result

The serial log updates acceleration in g and angular velocity in degrees/second every 500 ms.

## Troubleshooting

The display remains off; move the board to change the readings.
