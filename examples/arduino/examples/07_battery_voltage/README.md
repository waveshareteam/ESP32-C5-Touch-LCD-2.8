# Battery Voltage Display

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Read the connected battery voltage on the LCD and in the serial monitor.

## Hardware Required

ESP32-C5-Touch-LCD-2.8 and a USB data cable. Connect a compatible battery to the battery socket with correct polarity.

## How to Run

1. Complete the [Arduino first-use guide](../../../../docs/getting-started-arduino.md), then open **`07_battery_voltage.ino`** in this folder.
2. With power off, connect the battery with correct polarity, then connect USB. Select the board's port, click **Upload**, then open **Serial Monitor** at **115200 baud** and press RESET.
3. Read the voltage on the LCD; no touch operation is needed.

## Expected Result

The LCD and serial output update about once per second, showing volts and ADC 0-1023; this is voltage rather than remaining charge percentage.

## Troubleshooting

- **Zero or unusual voltage:** power off and check the battery connection and polarity; compare against a meter if necessary.
- **No charge percentage:** this example shows voltage only.
