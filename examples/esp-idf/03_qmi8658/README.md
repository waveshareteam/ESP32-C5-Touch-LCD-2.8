# QMI8658 Motion Sensor

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

See how acceleration and rotation readings change when you move the board. Results appear in the serial monitor; the LCD stays dark.

## Hardware Required

ESP32-C5-Touch-LCD-2.8 and a USB data cable; the QMI8658 is onboard and needs no external wiring.

## How to Run

1. Follow the [ESP-IDF first-use guide](../../../docs/getting-started-idf.md) to prepare the environment, then open **`03_qmi8658`** in an ESP-IDF terminal.
2. Run the commands below; the second uploads the example and opens the serial monitor at **115200 baud**.

   ```sh
   idf.py build
   idf.py flash monitor
   ```

3. Keep the serial monitor open. First place the board flat, then gently tilt and rotate it to compare the readings.

## Expected Result

Every 0.5 seconds the monitor shows `Accel [g]` (three acceleration axes) and `gyro [deg/s]` (three rotation axes). The numbers change as the board moves; a stationary board still measures gravity, so not all acceleration values will be zero.

## Troubleshooting

- **Why is the screen dark?** This example only uses the serial monitor; use [01_lvgl_demo](../01_lvgl_demo/) to test the screen.
- **No readings:** press RESET with the monitor open; check [USB and serial settings](../../../docs/getting-started-idf.md#troubleshooting).
