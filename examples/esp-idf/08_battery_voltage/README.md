# Battery Voltage Display

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Read the connected battery voltage on the LCD and in the serial monitor.

## Hardware Required

ESP32-C5-Touch-LCD-2.8, USB data cable and a compatible battery.

## How to Run

1. With power off, connect the battery with correct polarity, then connect USB. Follow the [ESP-IDF first-use guide](../../../docs/getting-started-idf.md) to prepare the environment, then open **`08_battery_voltage`** in an ESP-IDF terminal.
2. Run the commands below; the second uploads the example and opens the serial monitor at **115200 baud**.

   ```sh
   idf.py build
   idf.py flash monitor
   ```

3. Read the voltage on the LCD; no touch operation is needed.

## Expected Result

The LCD and serial monitor update about once per second with voltage, for example `3.900 V`, and an `ADC` reading. This is a voltage test; it does not display remaining charge percentage.

## Troubleshooting

- **Zero or unusual voltage:** power off and check the battery connection and polarity; use a meter when checking accuracy.
- **Why is there no percentage?** The example reports voltage only; it does not estimate remaining battery capacity.
