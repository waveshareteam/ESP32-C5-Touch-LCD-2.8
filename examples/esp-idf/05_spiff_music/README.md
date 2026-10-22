# SPIFFS Music

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Play the MP3 bundled in the SPIFFS image with an LVGL music interface.

## Hardware Required

ESP32-C5-Touch-LCD-2.8. The board and a connected speaker; no SD card is required.

## How to Run

From this directory, activate a supported ESP-IDF environment and run:

```sh
idf.py set-target esp32c5 build flash monitor
```

## Expected Result

The music interface appears and the speaker plays the bundled track.

## Troubleshooting

Flash the complete project, including the storage image generated from spiffs/.
