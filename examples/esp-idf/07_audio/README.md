# ES8311 Audio

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Records three seconds into PSRAM and plays it through the speaker once.

## Hardware Required

Board, USB data cable and speaker.

## How to Run

Activate an ESP-IDF version from [product.json](../../../.github/product.json), then run `idf.py set-target esp32c5`, `idf.py build` and `idf.py -p PORT flash monitor` in this directory. Keep the repository layout so the shared components can be found.

## Expected Result

Speak when the serial log says Speak now. The recording is played at 60% volume, then audio resources are released.

## Troubleshooting

No SD card is required. The single microphone uses the board's 24 kHz stereo-slot I2S profile; it has no hardware echo reference.
