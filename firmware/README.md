# Factory firmware

[English](README.md) | [简体中文](README_ZH.md)

| File | Purpose |
| --- | --- |
| [ESP32-C5-Touch-LCD-2.8_FactoryOnly_v1.1.bin](ESP32-C5-Touch-LCD-2.8_FactoryOnly_v1.1.bin) | Restore the supplied V1.1 demonstration; original merged image retained |

Connect the board over USB, install esptool, and flash this merged image from offset 0:

```sh
python -m pip install esptool
python -m esptool --chip esp32c5 write_flash 0x0 ESP32-C5-Touch-LCD-2.8_FactoryOnly_v1.1.bin
```

Reset the board after flashing. This legacy image keeps its V1.1 version name; factory application source is not included in this repository.
