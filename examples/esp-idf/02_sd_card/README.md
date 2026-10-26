# SD Card

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Mounts FAT at /sdcard, writes test.txt, reads it back and compares the contents.

## Hardware Required

Board, USB cable and FAT32 microSD card.

## How to Run

ESP32-C5 is selected by default. Activate ESP-IDF v5.5.4, then run `idf.py build` and `idf.py -p PORT flash monitor` in this directory. This project includes its own components and can be copied and built independently.

## Expected Result

The serial log reports write/read verification: ESP_OK.

## Troubleshooting

test.txt is overwritten. Insert the card before boot; formatting is never automatic.
