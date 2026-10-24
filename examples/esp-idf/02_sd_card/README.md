# SD Card

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Mounts FAT at /sdcard, writes test.txt, reads it back and compares the contents.

## Hardware Required

Board, USB cable and FAT32 microSD card.

## How to Run

Activate an ESP-IDF version from [product.json](../../../.github/product.json), then run `idf.py set-target esp32c5`, `idf.py build` and `idf.py -p PORT flash monitor` in this directory. Keep the repository layout so the shared components can be found.

## Expected Result

The serial log reports write/read verification: ESP_OK.

## Troubleshooting

test.txt is overwritten. Insert the card before boot; formatting is never automatic.
