# SD Card Detection

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Check whether the board can recognize a microSD card and display its information in the serial monitor; the LCD stays dark.

## Hardware Required

ESP32-C5-Touch-LCD-2.8 and a USB data cable. A FAT32 microSD test card. Back up its contents before use: this Arduino sketch may format the card if mounting fails.

## How to Run

1. Complete the [Arduino first-use guide](../../../../docs/getting-started-arduino.md), then open **`03_SD_Card.ino`** in this folder.
2. Insert the backed-up test card with power off, then connect USB. Select the board's port, click **Upload**, then open **Serial Monitor** at **115200 baud** and press RESET.
3. Wait about three seconds after RESET and read the card information.

## Expected Result

The monitor prints card information and a `practical_size` capacity value once after startup. There is no LCD interface and no test file is created; press RESET to run detection again.

## Troubleshooting

- **No card information:** reseat the card, confirm FAT32 formatting on a computer and press RESET; startup results are printed only once.
- **Where is test.txt?** This sketch only detects the card; use [IDF 02_sd_card](../../../esp-idf/02_sd_card/) for file write/read verification and LCD results.
