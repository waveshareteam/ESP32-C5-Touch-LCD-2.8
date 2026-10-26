# SHTC3 Temperature and Humidity

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Read the surrounding temperature and humidity in the serial monitor; the LCD stays dark.

## Hardware Required

ESP32-C5-Touch-LCD-2.8 and a USB data cable. The onboard sensor needs no external wiring.

## How to Run

1. Complete the [Arduino first-use guide](../../../../docs/getting-started-arduino.md), then open **`05_shtc3.ino`** in this folder.
2. Connect the board with a USB data cable. Select the board's port, click **Upload**, then open **Serial Monitor** at **115200 baud** and press RESET.
3. Leave the board in the room you want to measure and read temperature and humidity from the serial monitor.

## Expected Result

Serial output shows `Temperature` in C and `Humidity` in percent; the LCD stays dark.

## Troubleshooting

- **Dark screen:** read temperature and humidity in the serial monitor.
- **Failed to read SHTC3:** power-cycle the board; if the error persists, keep the serial log for support. Do not touch, cover or wet the sensor during measurement.
