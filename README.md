<div align="center">

# ESP32-C5-Touch-LCD-2.8

**2.8-inch ESP32-C5 Wi-Fi 6 touch display development board**

[![License](https://img.shields.io/github/license/waveshareteam/ESP32-C5-Touch-LCD-2.8)](./LICENSE)

[中文](./README.cn.md) | English | [Product](https://www.waveshare.com/catalogsearch/result/?q=ESP32-C5-Touch-LCD-2.8) | [Documentation](https://docs.waveshare.com/ESP32-C5-Touch-LCD-2.8) | [Firmware](./Firmware/) | [ESP-IDF Examples](./example/ESP-IDF-V554/) | [Arduino Examples](./example/Arduino-v3.3.10/example/)

<img src="./assets/ESP32-C5-Touch-LCD-2.8.png" alt="ESP32-C5-Touch-LCD-2.8" width="560">

</div>

## Overview

ESP32-C5-Touch-LCD-2.8 is a resource package for an ESP32-C5 based 2.8-inch touch LCD development board. This repository includes Arduino examples, ESP-IDF examples, factory firmware, schematics, mechanical files, SD card demo assets, and GUI Guider UI resources for quick hardware evaluation and application development.

This package is intended for:

- Verifying ESP32-C5, LCD, touch, sensors, SD card, and audio functions.
- Building touch LCD applications with Arduino or ESP-IDF.
- Reusing BSP, LVGL examples, and GUI Guider projects for UI development.
- Checking hardware schematics, mechanical dimensions, and factory firmware resources.

## Hardware Overview

| Item | Description |
| :--- | :--- |
| MCU | ESP32-C5 |
| Display | 2.8-inch LCD, 240 x 320 resolution |
| LCD driver | ST7789 |
| Touch controller | CST3530, I2C address `0x58` |
| IO expander | CH32V003, I2C address `0x24` |
| Sensors | QMI8658 6-axis IMU, PCF85063 RTC, SHTC3 temperature and humidity sensor |
| Storage assets | Micro SD card demo assets and SPIFFS audio demo |
| UI framework | Arduino examples use LVGL v8.4.0; ESP-IDF examples use LVGL v9 |
| Hardware files | [Schematic](./schematic/ESP32-C5-Touch-LCD-2.8.pdf), [dimensions](./dimensions/ESP32-C5-Touch-LCD-2.8.pdf), [DXF](./dimensions/ESP32-C5-Touch-LCD-2.8.dxf), and [STEP](./dimensions/ESP32-C5-Touch-LCD-2.8.stp) |

> CH32V003 is used for IO expansion, LCD reset, touch reset, and backlight control. Its control firmware is pre-programmed at the factory. In normal use, only the ESP32-C5 application needs to be flashed.

## Supported Toolchains

| Development framework | Version | Examples |
| :--- | :--- | ---: |
| Arduino-ESP32 | `3.3.10` | 6 |
| ESP-IDF | `v5.5.4` | 8 |

## Hardware Resources

| Resource | Download |
| :--- | :--- |
| Schematic | [ESP32-C5-Touch-LCD-2.8.pdf](./schematic/ESP32-C5-Touch-LCD-2.8.pdf) |
| Mechanical drawing | [PDF](./dimensions/ESP32-C5-Touch-LCD-2.8.pdf) |
| 2D drawing | [DXF](./dimensions/ESP32-C5-Touch-LCD-2.8.dxf) |
| 3D model | [STEP](./dimensions/ESP32-C5-Touch-LCD-2.8.stp) |

## Repository Layout

```text
.
├── dimensions/              # Mechanical files: PDF, DXF, STEP
├── assets/                  # Product images used by the documentation
├── example/
│   ├── Arduino-v3.3.10/     # Arduino examples and bundled libraries
│   └── ESP-IDF-V554/        # ESP-IDF v5.5.4 examples
├── Firmware/                # Factory firmware
├── guider_ui/               # GUI Guider installer / UI resources
├── schematic/               # Schematic
├── sdcard/                  # SD card test files, images, and audio assets
├── LICENSE
├── README.cn.md
└── README.en.md
```

## Arduino Quick Start

Recommended environment:

- Arduino IDE
- `esp32 by Espressif Systems v3.3.10`
- Board selection: `ESP32C5 Dev Module`

Steps:

1. Install `esp32 by Espressif Systems v3.3.10` from Arduino IDE Boards Manager.
2. Select `ESP32C5 Dev Module` from `Tools` > `Board`.
3. Copy the bundled libraries to the Arduino libraries directory:

   ```text
   example/Arduino-v3.3.10/lib/lvgl
   example/Arduino-v3.3.10/lib/SensorLib
   ```

   The default Arduino library directory on Windows is usually:

   ```text
   C:\Users\<UserName>\Documents\Arduino\libraries
   ```

4. Open an `.ino` example from `example/Arduino-v3.3.10/example`, then build and flash it.
5. It is recommended to run `01_exio` first to verify the CH32V003 IO expander before testing LCD, touch, and sensor examples.

### Arduino Examples

| Example | Function |
| :--- | :--- |
| [01_exio](./example/Arduino-v3.3.10/example/01_exio/) | CH32V003 IO expander test |
| [02_I2C_qmi8658](./example/Arduino-v3.3.10/example/02_I2C_qmi8658/) | QMI8658 6-axis IMU test |
| [03_SD_Card](./example/Arduino-v3.3.10/example/03_SD_Card/) | SD card read/write test |
| [04_I2C_pcf85063](./example/Arduino-v3.3.10/example/04_I2C_pcf85063/) | PCF85063 RTC test |
| [05_shtc3](./example/Arduino-v3.3.10/example/05_shtc3/) | SHTC3 temperature and humidity sensor test |
| [06_lvgl_demo](./example/Arduino-v3.3.10/example/06_lvgl_demo/) | LCD, touch, backlight, and LVGL basic UI test |

## ESP-IDF Quick Start

Recommended environment:

- ESP-IDF v5.5.4
- Target chip: `esp32c5`

Enter any ESP-IDF example directory and run:

```powershell
idf.py set-target esp32c5
idf.py build flash monitor
```

### ESP-IDF Examples

| Example | Function |
| :--- | :--- |
| [01_ch32_test](./example/ESP-IDF-V554/01_ch32_test/) | CH32V003 IO expander test, mainly observed through serial logs |
| [02_2048](./example/ESP-IDF-V554/02_2048/) | LVGL-based 2048 touch mini game |
| [03_sd_lvgl](./example/ESP-IDF-V554/03_sd_lvgl/) | SD card text reading and LVGL display |
| [04_sd_music](./example/ESP-IDF-V554/04_sd_music/) | MP3 playback from SD card |
| [05_spiff_music](./example/ESP-IDF-V554/05_spiff_music/) | MP3 music player demo using SPIFFS assets |
| [06_wifi_weather](./example/ESP-IDF-V554/06_wifi_weather/) | Wi-Fi weather demo |
| [07_Ball_qmi](./example/ESP-IDF-V554/07_Ball_qmi/) | QMI8658 motion-controlled ball demo |
| [08_bookesia](./example/ESP-IDF-V554/08_bookesia/) | Brookesia / drawing-board style touch application demo |

## SD Card Assets

The `sdcard/` directory contains test files, images, and audio assets used by the examples. For SD-card-related demos:

- Use a Micro SD card of 64 GB or smaller.
- Format the card as FAT32.
- Copy the files from `sdcard/` to the SD card root as required by each example.
- `04_sd_music` requires a `music` folder and `.mp3` files in the SD card root.

## Factory Firmware

Current version: **V1.1**

- [Download the factory firmware](./Firmware/ESP32-C5-Touch-LCD-2.8-FactoryAppV1.1.bin)
- Refer to the [product documentation](https://docs.waveshare.com/ESP32-C5-Touch-LCD-2.8) for the verified flashing procedure and flash settings.

It can be used to restore or verify the factory demo. Before flashing it, make sure the serial port, target chip, and flash settings are correct.

## Development Notes

- For common UI changes, start from the application layer, LVGL pages, GUI Guider generated code, or example business logic.
- Avoid changing BSP, low-level drivers, pin definitions, or partition tables unless the hardware connection or board design has changed.
- If LCD or touch does not work, verify CH32V003 communication first.
- The Arduino `06_lvgl_demo` example already adapts ST7789 and CST3530, so an extra GFX display library or third-party touch library is usually not required.
- The ESP-IDF BSP has already adapted LCD, touch, backlight, audio, sensors, and CH32 IO expansion.

## Notes

- Select `ESP32C5 Dev Module` for Arduino projects.
- The Arduino example folder is designed for `esp32 by Espressif Systems v3.3.10`.
- The `ESP-IDF-V554` folder is designed for ESP-IDF v5.5.4.
- CH32V003 firmware is pre-programmed at the factory and does not need to be flashed separately.
- For the weather demo, prepare an internet-connected Wi-Fi hotspot according to the example configuration.

## License

This repository is licensed under the Apache License 2.0. See [LICENSE](./LICENSE).

## Support

For product setup, development instructions, and troubleshooting, visit the [Waveshare documentation](https://docs.waveshare.com/ESP32-C5-Touch-LCD-2.8). When reporting an issue, include the example path, framework version, reproduction steps, and complete serial log.

- [Open an issue](https://github.com/waveshareteam/ESP32-C5-Touch-LCD-2.8/issues/new)
- [Waveshare support](https://www.waveshare.com/contact_us)
