# 出厂固件

[English](README.md) | [简体中文](README_ZH.md)

| 文件 | 用途 |
| --- | --- |
| [ESP32-C5-Touch-LCD-2.8_FactoryOnly_v1.1.bin](ESP32-C5-Touch-LCD-2.8_FactoryOnly_v1.1.bin) | 恢复 V1.1 出厂演示，保留原合并镜像 |

通过 USB 连接开发板，安装 esptool，从地址 0 烧录合并镜像：

```sh
python -m pip install esptool
python -m esptool --chip esp32c5 write_flash 0x0 ESP32-C5-Touch-LCD-2.8_FactoryOnly_v1.1.bin
```

烧录后复位开发板。此历史镜像保留 V1.1 版本命名；出厂应用源码暂未包含在本仓库中。
