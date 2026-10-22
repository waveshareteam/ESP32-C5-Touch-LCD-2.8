# SD Music

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Scan MP3 files on SD and play them through ES8311.

## Hardware Required

ESP32-C5-Touch-LCD-2.8. The board, a connected speaker and a FAT32 microSD card.

## How to Run

From this directory, activate a supported ESP-IDF environment and run:

```sh
idf.py set-target esp32c5 build flash monitor
```

## Expected Result

Serial output lists tracks and the speaker plays the selected file.

## Troubleshooting

Place MP3 files in the card's music directory; sample files are in [SD resources](../resources/sdcard/music/).
