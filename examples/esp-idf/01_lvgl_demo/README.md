# LVGL Display and Touch

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Try buttons, sliders and touch controls on the display.

## Hardware Required

ESP32-C5-Touch-LCD-2.8 and a USB data cable; no SD card or speaker is required.

## How to Run

1. Follow the [ESP-IDF first-use guide](../../../docs/getting-started-idf.md) to prepare the environment, then open **`01_lvgl_demo`** in an ESP-IDF terminal.
2. Run the commands below; the second uploads the example and opens the serial monitor at **115200 baud**.

   ```sh
   idf.py build
   idf.py flash monitor
   ```

3. Tap the on-screen controls and drag sliders; swipe the page to try the other widgets. Press RESET to restart the demonstration.

## Expected Result

The screen displays the LVGL widgets page. Buttons respond to taps and sliders follow your finger; this is a standalone demo, so there is no factory desktop to return to.

## Troubleshooting

- **Screen stays dark or touch does not respond:** press RESET and check for startup errors in the serial monitor; see the [connection checklist](../../../docs/getting-started-idf.md#troubleshooting).
