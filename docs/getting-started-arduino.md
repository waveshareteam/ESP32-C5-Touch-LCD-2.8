# Arduino: First Use

[English](getting-started-arduino.md) | [简体中文](getting-started-arduino_ZH.md)

Prepare the IDE once with this guide, then open the sketch named in the example README. Keep the complete downloaded repository so the supplied libraries are available.

## Prepare the IDE

1. Install [Arduino IDE](https://www.arduino.cc/en/software/) and extract this repository.
2. In **Preferences → Additional boards manager URLs**, add `https://espressif.github.io/arduino-esp32/package_esp32_index.json`.
3. Open **Boards Manager**, search for **esp32**, and install **esp32 by Espressif Systems 3.3.12**; the selected version is also listed in the [repository README](../README.md).
4. Set **Preferences → Sketchbook location** to the extracted repository's `examples/arduino` folder, then restart the IDE. Its `libraries` folder supplies the required libraries, including LVGL; do not install another LVGL version over it.

If you want to keep your existing sketchbook location, copy the contents of `examples/arduino/libraries` into that sketchbook's `libraries` folder instead, including `lv_conf.h`.

## Select the Board

Open the example's `.ino` file, connect the board with a USB data cable, and select the following **Tools** options:

| Option | Value |
| --- | --- |
| Board | ESP32C5 Dev Module |
| Flash Size | 16 MB |
| Flash Mode | QIO |
| PSRAM | Enabled |
| Partition Scheme | 16M Flash (3MB APP/9.9MB FATFS) |
| Port | The connected board's serial port |

Use these settings even when the module has a larger physical Flash capacity; they match the supplied examples.

## Upload and Observe

1. Prepare any speaker, battery or SD card required by the example README.
2. Click **Upload** and wait for completion.
3. Open **Serial Monitor**, select **115200 baud**, then press **RESET** to see messages from startup.
4. Follow **How to Run** and **Expected Result** in the example README. Some examples only print serial readings and leave the LCD dark.

Uploading an example replaces the factory desktop application; use the [factory firmware instructions](../firmware/README.md) when you want to restore it.

## Troubleshooting

| Symptom | What to do |
| --- | --- |
| No ESP32C5 board option | Check that the selected esp32 package version is installed. |
| Missing header or library | Check the sketchbook location and copied libraries, then restart the IDE. |
| LVGL compile error | Use the supplied libraries and `lv_conf.h`; check that another sketchbook/library location is not supplying a different LVGL. |
| No port or upload failure | Check the USB data cable, selected port and other serial programs; if stuck connecting, hold BOOT, press/release RESET, then release BOOT and retry. |
| No messages or garbled text | Select 115200 baud and press RESET with Serial Monitor open. |

If the issue persists, retain the sketch name and complete error/serial output for [technical support](https://service.waveshare.com/).
