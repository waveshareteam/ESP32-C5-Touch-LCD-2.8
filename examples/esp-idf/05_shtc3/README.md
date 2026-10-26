# SHTC3 Temperature and Humidity

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Read room temperature and humidity in the serial monitor. The LCD stays dark.

## Hardware Required

ESP32-C5-Touch-LCD-2.8 and a USB data cable; the sensor is onboard and no SD card is required.

## How to Run

1. Follow the [ESP-IDF first-use guide](../../../docs/getting-started-idf.md) to prepare the environment, then open **`05_shtc3`** in an ESP-IDF terminal.
2. Run the commands below; the second uploads the example and opens the serial monitor at **115200 baud**.

   ```sh
   idf.py build
   idf.py flash monitor
   ```

3. Leave the board in the room you want to measure and read the serial monitor. No screen operation is needed.

## Expected Result

The monitor updates once per second, for example `25.00 C; 50.00 %RH`: 25 degrees Celsius and 50% relative humidity. Readings vary with the surroundings.

## Troubleshooting

- **Why is the screen dark?** Temperature and humidity appear only in the serial monitor.
- **Readings seem unusual:** avoid touching, covering or wetting the sensor; if a read error persists after a power cycle, retain the serial log for support.
