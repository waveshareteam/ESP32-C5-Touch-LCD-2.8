# Weather GUI Sources

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

GUI Guider project and simulator resources for the weather interface. This is an editor project, not an ESP-IDF firmware example.

## Hardware Required

A computer with NXP GUI Guider for editing; the board and a Wi-Fi network are needed to retrieve weather.

## How to Run

Open `weather_app.guiguider` in GUI Guider. Configure, build and flash the board application from [06_wifi_weather](../../06_wifi_weather/).

## Expected Result

The editor shows the weather layout; the ESP-IDF example fills it with retrieved weather data.

## Troubleshooting

Editing these assets does not update the firmware automatically. Export the UI and integrate it into the example while preserving its LVGL version.
