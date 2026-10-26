# CH32V003 IO

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Toggles header EXIO6/7 and the backlight PWM once per second.

## Hardware Required

Board and USB cable; optionally connect a meter to EXIO6/7.

## How to Run

ESP32-C5 is selected by default. Activate ESP-IDF v5.5.4, then run `idf.py build` and `idf.py -p PORT flash monitor` in this directory. This project includes its own components and can be copied and built independently.

## Expected Result

EXIO6/7 alternate high/low; the log reports output level and PWM value.

## Troubleshooting

The LCD is not initialized. Only EXIO6/7 are test outputs; EXIO0/1/3 control reset/amplifier and EXIO4/5 serve the sensor.
