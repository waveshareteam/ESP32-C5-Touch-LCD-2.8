# Factory Application Project

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Provides the 240 × 320 desktop with 12 applications, including AIChats, SD music, photos, microphone, display and button tests. App operation and Wi-Fi setup are described in the [user guide](USER_GUIDE.md).

## Hardware Required

ESP32-C5-Touch-LCD-2.8, USB data cable and a speaker for audio. Music and Photos additionally need a FAT32 microSD card; Mic Test records into PSRAM and does not need a card.

## How to Run

1. Install and activate ESP-IDF **v5.5.4**. Open this `factory` folder in VS Code or an ESP-IDF terminal; ESP32-C5 is selected by default.
2. Connect the USB data cable and, for audio, the speaker. For Music and Photos, prepare a FAT32 microSD card with `/music` MP3 files and `/photo` JPEG images and insert it before powering on.
3. Run the commands below in this folder. `flash` uploads all generated images, including the speech model; uploading the application BIN alone is insufficient.

   ```sh
   idf.py build
   idf.py flash monitor
   ```

4. The monitor uses **2,000,000 baud** from the project configuration. If multiple ports exist, use `idf.py -p PORT flash monitor` with the board's port; press `Ctrl+]` to leave the monitor.

To restore a tested desktop without compiling, use the [factory firmware instructions](../../firmware/README.md).

## Expected Result

The screen displays the wallpaper, white application names and centered time. Swipe left/right to change pages, tap an icon to open it, and swipe upward from the bottom center to return; use Settings to configure Wi-Fi and the user guide for individual applications.

## Troubleshooting

- No Music/Photos files: check the card folders and file types, then reopen the application. Mic Test uses Record/Stop/Play and can retain up to 10 seconds until the application is closed.
- Build/connection problems: use v5.5.4, clear an old build with `idf.py fullclean` after switching SDK, and check the selected port and monitor baud.
