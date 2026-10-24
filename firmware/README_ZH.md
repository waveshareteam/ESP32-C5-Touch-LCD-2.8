# 出厂固件

[English](README.md) | [简体中文](README_ZH.md)

适用于 ESP32-C5-Touch-LCD-2.8 的可直接烧录固件。每个文件都是从 `0x0` 烧录的合并镜像，文件名中的日期或版本号用于区分版本；旧版保留，最新版排在最前。

| 文件 | 用途 |
| --- | --- |
| [ESP32-C5-Touch-LCD-2.8_FactoryOnly_20261024.bin](ESP32-C5-Touch-LCD-2.8_FactoryOnly_20261024.bin) | 当前 12 应用桌面，支持 2.4/5 GHz Wi-Fi、集成式 AIChats、SD 音乐、屏幕测试和按键测试 |
| [ESP32-C5-Touch-LCD-2.8_FactoryOnly_20261022.bin](ESP32-C5-Touch-LCD-2.8_FactoryOnly_20261022.bin) | 上一版出厂镜像，保留供旧设备恢复使用 |
| [ESP32-C5-Touch-LCD-2.8_FactoryOnly_v1.1.bin](ESP32-C5-Touch-LCD-2.8_FactoryOnly_v1.1.bin) | 历史 V1.1 演示 |

使用 esptool 烧录：

```sh
python -m pip install esptool
python -m esptool --chip esp32c5 --port PORT erase_flash
python -m esptool --chip esp32c5 --port PORT write_flash 0x0 ESP32-C5-Touch-LCD-2.8_FactoryOnly_20261024.bin
```

在本目录运行，将 `PORT` 替换为设备实际串口，完成后复位开发板。最新版使用 16 MB Flash 布局，包含引导程序、分区表、OTA 元数据、应用和语音模型；擦除 Flash 会清除已保存的设置。

出厂源码位于 [examples/factory](../examples/factory/README_ZH.md)，配网及应用操作见[使用说明](../examples/factory/USER_GUIDE_ZH.md)。
