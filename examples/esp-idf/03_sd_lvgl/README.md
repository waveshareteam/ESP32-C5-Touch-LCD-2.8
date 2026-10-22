# SD Text Display

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Read a text file from SD and show it using LVGL.

## Hardware Required

ESP32-C5-Touch-LCD-2.8. The board and a FAT32 microSD card with Test.txt at its root.

## How to Run

From this directory, activate a supported ESP-IDF environment and run:

```sh
idf.py set-target esp32c5 build flash monitor
```

## Expected Result

The display shows the text or a missing-card/file message.

## Troubleshooting

Use the supplied [SD resources](../resources/sdcard/); keep the file name Test.txt.
