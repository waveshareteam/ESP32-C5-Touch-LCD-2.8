# Battery Voltage Display

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Averages eight CH32V003 ADC readings and displays the estimated battery voltage and raw ADC value on LCD and serial output.

## Hardware Required

ESP32-C5-Touch-LCD-2.8, USB data cable and a compatible battery connected to the board battery socket with correct polarity.

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

The LCD and serial monitor update approximately every second, showing voltage in volts and `ADC: ... / 1023`. This example shows voltage, not charge percentage.

## Troubleshooting

- A missing battery or poor connection can produce zero/abnormal readings. The estimate uses a nominal 3.3 V ADC reference and a 3:1 divider; compare with a meter when checking accuracy.
- No serial output: check the USB data cable and selected port, close other serial tools, then press RESET. The monitor rate is 115200 baud.
- Build error after changing SDK versions: select ESP-IDF **v5.5.4**, run `idf.py fullclean`, then rebuild. This repository uses the 5.5.x driver APIs; ESP-IDF 6.x is not the selected environment.
