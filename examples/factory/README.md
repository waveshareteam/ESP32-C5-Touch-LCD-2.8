# Factory Application Project

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Provides a 240 × 320 portrait desktop with 12 applications, including integrated AIChats, SD music, photos, display and button tests. See the [user guide](USER_GUIDE.md) for application controls and network setup.

## Hardware Required

Use an ESP32-C5-Touch-LCD-2.8, USB data cable and speaker. Music and Photos require a FAT32 microSD card with `/music` MP3 files and `/photo` JPEG images; Mic Test uses PSRAM.

## How to Run

Activate ESP-IDF v5.5.4, then run `idf.py set-target esp32c5`, `idf.py build` and `idf.py -p PORT flash monitor` here. The project selects LVGL 9.5.0 and references the shared BSP in ../esp-idf/components (keep the repository layout); flash all generated segments, including the speech model.

## Expected Result

The original wallpaper appears with white desktop labels and a centered clock; the backlight turns on after the first complete frame. Display Test cycles five colors, Button Test checks BOOT, and the other applications share the board's display, SD card and audio resources.

## Troubleshooting

Music reads the SD card, not SPIFFS; use `/music` and reopen the application after inserting a card. The separately distributed [firmware images](../../firmware/README.md) retain their original version's features and are not replaced by a source update; the serial monitor uses 2,000,000 baud.
