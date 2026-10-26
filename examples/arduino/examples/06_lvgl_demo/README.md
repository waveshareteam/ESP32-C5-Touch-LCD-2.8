# LVGL Display and Touch

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Check the displayed touch coordinates and adjust screen brightness with a slider.

## Hardware Required

ESP32-C5-Touch-LCD-2.8 and a USB data cable. No SD card or speaker is required.

## How to Run

1. Complete the [Arduino first-use guide](../../../../docs/getting-started-arduino.md), then open **`06_lvgl_demo.ino`** in this folder.
2. Connect the board with a USB data cable. Select the board's port, click **Upload**, then open **Serial Monitor** at **115200 baud** and press RESET.
3. Touch different areas of the screen to check coordinates, then drag the bottom slider to adjust brightness.

## Expected Result

The LCD shows Display OK and touch coordinates. Touch the screen and drag the bottom slider to adjust backlight from 5% to 100%.

## Troubleshooting

- **Screen works but coordinates do not change:** check whether the screen says `Touch: not found`, then press RESET and retain startup errors if it persists.
- **Missing library or LVGL errors:** follow the [bundled-library setup](../../../../docs/getting-started-arduino.md#prepare-the-ide) rather than installing a different LVGL version.
