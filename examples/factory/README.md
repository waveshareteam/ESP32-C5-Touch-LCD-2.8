# Factory Desktop Demonstration

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Try the board through a touchscreen desktop with 12 applications, including voice chat, weather, SD music, photos and hardware tests. See the [application guide](USER_GUIDE.md) for each app.

## Hardware Required

ESP32-C5-Touch-LCD-2.8 and a USB data cable. Connect a speaker for audio; prepare a FAT32 microSD card for Music and Photos, and an internet connection for weather and AIChats.

## How to Run

1. To try the desktop directly, follow the [factory firmware instructions](../../firmware/README.md) to flash the latest image, then press RESET. To upload from source, follow the [ESP-IDF guide](../../docs/getting-started-idf.md) and run `idf.py build` followed by `idf.py flash monitor` in this folder; the factory monitor uses **2,000,000 baud**.
2. Swipe left/right to change desktop pages and tap an app to open it. Swipe up from the bottom center to return; in Display Test, double-tap anywhere to exit.
3. Open **Settings → Wi-Fi setup** and follow the [Wi-Fi steps](USER_GUIDE.md#connecting-wi-fi) before using online apps. For Music and Photos, place MP3 files in the card's `/music` folder and JPG/JPEG images in `/photo`, then insert the card before power-on.

## Expected Result

The desktop shows app icons and a clock; the clock fills in after connecting to the internet. Open Mic Test to record and replay speech, or Display Test to cycle red, green, blue, yellow and white.

## Troubleshooting

- **No music or photos:** check FAT32 formatting, folder names and file types; reopen the app after inserting a card.
- **Cannot use AIChats or weather:** confirm that Wi-Fi has internet access, then follow the [application guide](USER_GUIDE.md).
- **How do I leave an app?** Swipe upward from the bottom center; for Display Test, double-tap anywhere. Music stops when you leave the player.
