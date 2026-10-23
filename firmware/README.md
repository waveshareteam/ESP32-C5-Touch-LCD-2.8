# Factory firmware

[English](README.md) | [简体中文](README_ZH.md)

| File | Purpose |
| --- | --- |
| [ESP32-C5-Touch-LCD-2.8_FactoryOnly_20261022.bin](ESP32-C5-Touch-LCD-2.8_FactoryOnly_20261022.bin) | Current factory desktop: integrated AIChats, 10-second Mic Test and voice volume/brightness control; includes SPIFFS and the speech model |
| [ESP32-C5-Touch-LCD-2.8_FactoryOnly_v1.1.bin](ESP32-C5-Touch-LCD-2.8_FactoryOnly_v1.1.bin) | Historical V1.1 demo, retained for restoring the previous firmware |

The latest distributed image is listed first. The [factory source](../examples/factory/) has since been updated to the 12-application desktop with SD music; the BIN files below have not been replaced by that source update. Older images remain available for restoring older devices. Back up any files and settings you need before erasing; the image replaces the partition layout and bundled resources.

```sh
python -m pip install esptool
python -m esptool --chip esp32c5 --port PORT erase_flash
python -m esptool --chip esp32c5 --port PORT write_flash 0x0 ESP32-C5-Touch-LCD-2.8_FactoryOnly_20261022.bin
```

Run these commands in this directory, replacing `PORT` with the device port, then reset the board. The image uses a 16 MB Flash layout.

The latest image is merged from the original hardware-tested build, including its bootloader, partition table, OTA metadata, application, SPIFFS and speech model. SHA-256: `7b47c2a622077fb7b36276ae63611ddfced58be23237526d341c7fb3b6d9816a`.

Source and build instructions: [examples/factory](../examples/factory/).
