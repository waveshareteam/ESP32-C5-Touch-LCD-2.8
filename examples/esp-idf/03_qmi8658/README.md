# QMI8658 IMU

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Reads acceleration and angular velocity through the local BSP.

## Hardware Required

Board and USB data cable.

## How to Run

Activate ESP-IDF v5.5.4, then run `idf.py set-target esp32c5`, `idf.py build` and `idf.py -p PORT flash monitor` in this directory. This project includes its own components and can be copied and built independently.

## Expected Result

The serial log updates acceleration in g and angular velocity in degrees/second every 500 ms.

## Troubleshooting

The display remains off; move the board to change the readings.
