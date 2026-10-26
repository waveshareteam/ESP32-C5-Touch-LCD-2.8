# QMI8658 Motion Sensor

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

See how movement changes the acceleration and rotation readings in the serial monitor; the LCD stays dark.

## Hardware Required

ESP32-C5-Touch-LCD-2.8 and a USB data cable. The onboard sensor needs no external wiring.

## How to Run

1. Complete the [Arduino first-use guide](../../../../docs/getting-started-arduino.md), then open **`02_I2C_qmi8658.ino`** in this folder.
2. Connect the board with a USB data cable. Select the board's port, click **Upload**, then open **Serial Monitor** at **115200 baud** and press RESET.
3. Place the board flat, then gently tilt and rotate it while watching the serial readings.

## Expected Result

Serial lines show x/y/z acceleration and gyro_x/gyro_y/gyro_z; gently rotate the board to see changes. The LCD stays dark.

## Troubleshooting

- **Dark screen:** results appear only in the serial monitor.
- **Acceleration is not zero at rest:** the sensor still measures gravity; tilt the board and compare the changes.
