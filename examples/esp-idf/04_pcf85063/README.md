# PCF85063 Real-Time Clock

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Watch the onboard clock advance from a preset demonstration date. Results appear in the serial monitor; the LCD stays dark.

## Hardware Required

ESP32-C5-Touch-LCD-2.8 and a USB data cable; no external RTC wiring is required.

## How to Run

1. Follow the [ESP-IDF first-use guide](../../../docs/getting-started-idf.md) to prepare the environment, then open **`04_pcf85063`** in an ESP-IDF terminal.
2. Run the commands below; the second uploads the example and opens the serial monitor at **115200 baud**.

   ```sh
   idf.py build
   idf.py flash monitor
   ```

3. Watch the seconds advance in the serial monitor. Press RESET to run the demonstration again from its preset date.

## Expected Result

Time starts at **2026-10-24 12:00:00** and updates once per second. RESET or power-on sets that date again; this demonstration does not show the current internet time.

## Troubleshooting

- **Why is the date different from today?** The preset date is intentional and is restored on each boot; it is not an indication that the clock failed.
- **Why is the screen dark?** View the time in the serial monitor; this example has no screen interface.
