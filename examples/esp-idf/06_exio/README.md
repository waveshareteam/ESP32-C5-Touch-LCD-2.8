# CH32V003 IO Expander

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Check the EXIO6/7 header outputs and watch the backlight switch on and off once per second.

## Hardware Required

ESP32-C5-Touch-LCD-2.8 and a USB data cable. Optionally measure EXIO6/7 against board GND with a multimeter; no external wiring is needed for the backlight test.

## How to Run

1. Follow the [ESP-IDF first-use guide](../../../docs/getting-started-idf.md) to prepare the environment, then open **`06_exio`** in an ESP-IDF terminal.
2. Run the commands below; the second uploads the example and opens the serial monitor at **115200 baud**.

   ```sh
   idf.py build
   idf.py flash monitor
   ```

3. Observe the backlight turning on and off. Optionally measure EXIO6/7 relative to board GND to check the output levels.

## Expected Result

The backlight alternates between dark and bright; no picture or touch interface is displayed. Serial output alternates `EXIO6/7=0; backlight PWM=0` and `EXIO6/7=1; backlight PWM=255`.

## Troubleshooting

- **Backlight flashes but there is no picture:** this is expected; use [01_lvgl_demo](../01_lvgl_demo/) for a display interface.
- **Which pins should I measure?** Use only the header EXIO6/7 and GND; see the [header guide](../../../docs/ch32v003.md).
