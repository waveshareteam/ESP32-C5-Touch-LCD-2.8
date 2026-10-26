# PCF85063 Real-Time Clock

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Sets a demonstration time on boot and reads the onboard RTC once per second.

## Hardware Required

ESP32-C5-Touch-LCD-2.8 and a USB data cable. The onboard RTC needs no external wiring.

## How to Run

1. Install Arduino IDE and add `https://espressif.github.io/arduino-esp32/package_esp32_index.json` to **Preferences → Additional boards manager URLs**. In Boards Manager, install **esp32 by Espressif Systems 3.3.12**, the version selected in [product.json](../../../../.github/product.json).
2. Set **Preferences → Sketchbook location** to this repository's `examples/arduino` folder and restart the IDE so it finds the bundled [libraries](../../libraries/). Open this folder's `.ino` file.
3. Under **Tools**, select **ESP32C5 Dev Module**, Flash Size **16 MB**, Flash Mode **QIO**, PSRAM **Enabled**, and Partition Scheme **16M Flash (3MB APP/9.9MB FATFS)**. Connect a USB data cable and choose the board's serial port.
4. Click **Upload**, open **Serial Monitor** at **115200 baud**, then press RESET to see startup messages.

## Expected Result

Serial time starts at 2025/9/9 14:51:30 and advances every second; the LCD stays dark. RESET sets the demonstration time again.

## Troubleshooting

- To change the initial date, edit the i2c_rtc_setTime call in the sketch and upload again; this example does not synchronize time over Wi-Fi.
- Missing header/library: confirm the sketchbook folder and bundled libraries are in place, then restart Arduino IDE. Do not replace the bundled LVGL 8.4 with an unrelated major version.
- Upload/no output: check the USB data cable and Tools → Port; close other serial programs and confirm 115200 baud before pressing RESET.
