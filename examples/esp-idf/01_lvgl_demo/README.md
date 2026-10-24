# LVGL Display Demo

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Runs the LVGL widgets demo with CST3530 touch. Backlight is enabled after the first frame.

## Hardware Required

Board and USB data cable.

## How to Run

Activate an ESP-IDF version from [product.json](../../../.github/product.json), then run `idf.py set-target esp32c5`, `idf.py build` and `idf.py -p PORT flash monitor` in this directory. Keep the repository layout so the shared components can be found.

## Expected Result

The widgets page appears and responds to touch.

## Troubleshooting

This example starts display and touch only; sensor, SD and audio initialization is not required.
