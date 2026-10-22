# Wi-Fi Weather

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Connect to Wi-Fi and show periodically refreshed weather data.

## Hardware Required

ESP32-C5-Touch-LCD-2.8. The board and a Wi-Fi network with Internet access.

## How to Run

From this directory, activate a supported ESP-IDF environment and run:

```sh
idf.py set-target esp32c5 build flash monitor
```

## Expected Result

The screen shows the weather and serial output reports connection/update results.

## Troubleshooting

Check Wi-Fi and weather options in components/generated/events_init.c before building.
