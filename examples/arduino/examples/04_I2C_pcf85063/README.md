# PCF85063 Real-Time Clock

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Watch the onboard clock count from a preset date in the serial monitor; the LCD stays dark.

## Hardware Required

ESP32-C5-Touch-LCD-2.8 and a USB data cable. The onboard RTC needs no external wiring.

## How to Run

1. Complete the [Arduino first-use guide](../../../../docs/getting-started-arduino.md), then open **`04_I2C_pcf85063.ino`** in this folder.
2. Connect the board with a USB data cable. Select the board's port, click **Upload**, then open **Serial Monitor** at **115200 baud** and press RESET.
3. Watch the seconds increase in the serial monitor; press RESET to repeat from the preset date.

## Expected Result

Serial time starts at 2025/9/9 14:51:30 and advances every second; the LCD stays dark. RESET sets the demonstration time again.

## Troubleshooting

- **Date is not today:** this example always resets to its preset date on startup; it does not obtain internet time.
- **Dark screen:** read the clock in the serial monitor.
