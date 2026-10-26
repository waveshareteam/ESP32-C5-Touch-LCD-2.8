# LVGL Display and Touch

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Displays a touch interface and brightness slider using bundled LVGL 8.4.

## Hardware Required

ESP32-C5-Touch-LCD-2.8 and a USB data cable. No SD card or speaker is required.

## How to Run

1. Install Arduino IDE and add `https://espressif.github.io/arduino-esp32/package_esp32_index.json` to **Preferences → Additional boards manager URLs**. In Boards Manager, install **esp32 by Espressif Systems 3.3.12**, the version selected in [product.json](../../../../.github/product.json).
2. Set **Preferences → Sketchbook location** to this repository's `examples/arduino` folder and restart the IDE so it finds the bundled [libraries](../../libraries/). Open this folder's `.ino` file.
3. Under **Tools**, select **ESP32C5 Dev Module**, Flash Size **16 MB**, Flash Mode **QIO**, PSRAM **Enabled**, and Partition Scheme **16M Flash (3MB APP/9.9MB FATFS)**. Connect a USB data cable and choose the board's serial port.
4. Click **Upload**, open **Serial Monitor** at **115200 baud**, then press RESET to see startup messages.

## Expected Result

The LCD shows Display OK and touch coordinates. Touch the screen and drag the bottom slider to adjust backlight from 5% to 100%.

## Troubleshooting

- Use the bundled libraries/lv_conf.h; color configuration requires 16-bit depth and LV_COLOR_16_SWAP=0. Check serial errors if the display or touch is unavailable.
- Missing header/library: confirm the sketchbook folder and bundled libraries are in place, then restart Arduino IDE. Do not replace the bundled LVGL 8.4 with an unrelated major version.
- Upload/no output: check the USB data cable and Tools → Port; close other serial programs and confirm 115200 baud before pressing RESET.
