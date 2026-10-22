# Brookesia Drawing Board

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Run the Brookesia desktop and its touch drawing application.

## Hardware Required

ESP32-C5-Touch-LCD-2.8. The board and a USB cable.

## How to Run

From this directory, activate a supported ESP-IDF environment and run:

```sh
idf.py set-target esp32c5 build flash monitor
```

## Expected Result

The desktop opens the drawing application and touch strokes appear on screen.

## Troubleshooting

This example contains the retained desktop/drawing demo, not the separate AIChats factory application.
