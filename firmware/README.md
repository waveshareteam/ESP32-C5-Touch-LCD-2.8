# Factory Firmware

[English](README.md) | [简体中文](README_ZH.md)

Ready-to-flash firmware for ESP32-C5-Touch-LCD-2.8. Each file is a merged image flashed at `0x0`; the date or version in its name identifies the release. Older versions are retained, with the newest listed first.

| File | Purpose |
| --- | --- |
| [ESP32-C5-Touch-LCD-2.8_FactoryOnly_20261024.bin](ESP32-C5-Touch-LCD-2.8_FactoryOnly_20261024.bin) | Current 12-application desktop with 2.4/5 GHz Wi-Fi, integrated AIChats, SD music, Display Test and Button Test |
| [ESP32-C5-Touch-LCD-2.8_FactoryOnly_20261022.bin](ESP32-C5-Touch-LCD-2.8_FactoryOnly_20261022.bin) | Previous factory image, retained for restoring older devices |
| [ESP32-C5-Touch-LCD-2.8_FactoryOnly_v1.1.bin](ESP32-C5-Touch-LCD-2.8_FactoryOnly_v1.1.bin) | Historical V1.1 demo |

Flash with esptool:

```sh
python -m pip install esptool
python -m esptool --chip esp32c5 --port PORT erase_flash
python -m esptool --chip esp32c5 --port PORT write_flash 0x0 ESP32-C5-Touch-LCD-2.8_FactoryOnly_20261024.bin
```

Run these commands in this directory, replacing `PORT` with the device port, then reset the board. The latest image uses a 16 MB Flash layout and includes the bootloader, partition table, OTA metadata, application and speech model. Erasing Flash removes saved settings.

The factory source is in [examples/factory](../examples/factory/); see the [user guide](../examples/factory/USER_GUIDE.md) for network setup and application controls.
