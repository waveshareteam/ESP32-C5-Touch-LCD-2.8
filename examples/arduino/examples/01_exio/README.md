# IO Expander

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Check the EXIO6/7 header outputs and watch the backlight change once per second.

## Hardware Required

ESP32-C5-Touch-LCD-2.8 and a USB data cable. Optionally use a multimeter between EXIO6/7 and board GND.

## How to Run

1. Complete the [Arduino first-use guide](../../../../docs/getting-started-arduino.md), then open **`01_exio.ino`** in this folder.
2. Connect the board with a USB data cable. Select the board's port, click **Upload**, then open **Serial Monitor** at **115200 baud** and press RESET.
3. Watch the backlight switch on and off; optionally measure EXIO6/7 against GND.

## Expected Result

Serial output alternates EXIO6/7 levels and backlight 0%/100%. The backlight changes, but no LCD UI is initialized.

## Troubleshooting

- **No picture:** this test only switches the backlight, so no picture is expected.
- **Which pins should I measure?** Use the header EXIO6/7 and GND; see the [header guide](../../../../docs/ch32v003.md).
