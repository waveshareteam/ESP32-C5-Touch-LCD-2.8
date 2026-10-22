# Factory Application Project

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Provides a 240 × 320 portrait Brookesia desktop with Settings, AIChats, Sensor, Drawing, the IMU ball, MusicPlayer, Weather, Games, Photos and Mic Test. AIChats runs in the same firmware without switching boot partitions.

## Hardware Required

Use an ESP32-C5-Touch-LCD-2.8 board, USB data cable and speaker. Photos requires a FAT32 microSD card; the bundled music and microphone recording test do not require one.

## How to Run

For the ready-to-flash image, follow the [firmware instructions](../../firmware/README.md). To build from source, enable ESP-IDF v5.5.4 and run `idf.py set-target esp32c5`, `idf.py build` and `idf.py -p PORT flash monitor` in this directory; LVGL 9.5.0 is selected by the manifest.

## Expected Result

The board opens its application desktop; MusicPlayer plays the bundled SPIFFS MP3, and Mic Test records up to 10 seconds in PSRAM for speaker playback. Follow the [user guide](USER_GUIDE.md) to configure Wi-Fi, activate AIChats and use each application.

## Troubleshooting

Flash the complete image, including SPIFFS and the speech model, when upgrading from V1.1; an application-only BIN is insufficient. Weather and AIChats require internet access; the serial monitor uses 2,000,000 baud.
