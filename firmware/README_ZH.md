# 出厂固件

[English](README.md) | [简体中文](README_ZH.md)

| 文件 | 用途 |
| --- | --- |
| [ESP32-C5-Touch-LCD-2.8_FactoryOnly_20261022.bin](ESP32-C5-Touch-LCD-2.8_FactoryOnly_20261022.bin) | 当前出厂桌面，集成 AIChats、10 秒 Mic Test 和语音音量/亮度调节；包含 SPIFFS 资源与语音模型 |
| [ESP32-C5-Touch-LCD-2.8_FactoryOnly_v1.1.bin](ESP32-C5-Touch-LCD-2.8_FactoryOnly_v1.1.bin) | 历史 V1.1 演示，仅用于恢复旧版固件 |

新版排在首位，当前[使用说明](../examples/factory/USER_GUIDE_ZH.md)对应新版镜像；旧版本保留供旧设备恢复使用。擦除前请备份需要的文件及设置，新版会替换分区布局和内置资源。

```sh
python -m pip install esptool
python -m esptool --chip esp32c5 --port PORT erase_flash
python -m esptool --chip esp32c5 --port PORT write_flash 0x0 ESP32-C5-Touch-LCD-2.8_FactoryOnly_20261022.bin
```

在本目录运行，将 `PORT` 替换为设备实际串口，完成后复位开发板。镜像使用 16 MB Flash 布局。

新版由原有已实测构建文件合并，包含引导程序、分区表、OTA 元数据、应用、SPIFFS 和语音模型。SHA-256：`7b47c2a622077fb7b36276ae63611ddfced58be23237526d341c7fb3b6d9816a`。

源码与编译说明：[examples/factory](../examples/factory/README_ZH.md)。
