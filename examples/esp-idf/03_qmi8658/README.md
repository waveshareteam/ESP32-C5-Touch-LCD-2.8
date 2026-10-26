# QMI8658 Motion Sensor

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Reads the onboard accelerometer and gyroscope and prints their three axes to the serial monitor every 500 ms.

## Hardware Required

ESP32-C5-Touch-LCD-2.8 and a USB data cable; the QMI8658 is onboard and needs no external wiring.

## How to Run

1. Install and activate [ESP-IDF v5.5.4](https://docs.espressif.com/projects/esp-idf/en/v5.5.4/esp32c5/get-started/). In VS Code, select that installed environment and open this example folder, containing `CMakeLists.txt`; ESP32-C5 is selected automatically.
2. Connect the board to the computer with a USB data cable. Open an ESP-IDF terminal **in this example folder** and run:

   ```sh
   idf.py build
   idf.py flash monitor
   ```

3. The second command uploads the program and opens serial output at **115200 baud**. If multiple serial ports exist, use `idf.py -p PORT flash monitor`, replacing `PORT` with the board's port shown in Device Manager; press `Ctrl+]` to exit the monitor.

The folder includes its BSP and local drivers and can be copied independently. Uploading an example replaces the running application; the [factory firmware](../../../firmware/README.md) restores the desktop.

## Expected Result

Serial lines contain `Accel [g]` and `gyro [deg/s]`. Gently tilt or rotate the board and observe the axes change; the LCD remains dark because this example does not initialize it.

## Troubleshooting

- Sensor read/init errors: check the reported I2C error and power-cycle the board. A board at rest still measures gravity; acceleration is not expected to be zero on every axis.
- No serial output: check the USB data cable and selected port, close other serial tools, then press RESET. The monitor rate is 115200 baud.
- Build error after changing SDK versions: select ESP-IDF **v5.5.4**, run `idf.py fullclean`, then rebuild. This repository uses the 5.5.x driver APIs; ESP-IDF 6.x is not the selected environment.
