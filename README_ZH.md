<div align="center">
  <h1>ESP32-C5-Touch-LCD-2.8</h1>
  <p><strong>2.8 英寸 240 × 320 ESP32-C5 Wi-Fi 6 触摸显示开发板</strong></p>
  <p><a href="README.md">English</a> | <strong>简体中文</strong></p>
  <p>
    <a href="https://github.com/waveshareteam/ESP32-C5-Touch-LCD-2.8/actions/workflows/build.yml"><img alt="构建" src="https://github.com/waveshareteam/ESP32-C5-Touch-LCD-2.8/actions/workflows/build.yml/badge.svg"></a>
    <a href="https://github.com/waveshareteam/ESP32-C5-Touch-LCD-2.8/releases/latest"><img alt="最新版本" src="https://img.shields.io/github/v/release/waveshareteam/ESP32-C5-Touch-LCD-2.8"></a>
    <a href="LICENSE"><img alt="许可证" src="https://img.shields.io/github/license/waveshareteam/ESP32-C5-Touch-LCD-2.8"></a>
  </p>
  <p>
    <a href="https://www.waveshare.com/ESP32-C5-Touch-LCD-2.8.htm">🌐 产品</a> &middot;
    <a href="https://github.com/waveshareteam/ESP32-C5-Touch-LCD-2.8/releases/latest">📦 固件</a> &middot;
    <a href="examples/esp-idf/">🧩 ESP-IDF</a> &middot;
    <a href="examples/arduino/examples/">🔧 Arduino</a> &middot;
    <a href="hardware/">📚 文档</a>
  </p>
  <p><img src="docs/images/ESP32-C5-Touch-LCD-2.8.jpg" alt="ESP32-C5-Touch-LCD-2.8 开发板" width="500"></p>
</div>

---

## ✨ 概述

本仓库提供 ESP32-C5 触摸显示开发板的示例和硬件资料，涵盖显示、触摸、音频、SD 卡与传感器。CH32V003 提供 IO 扩展和背光 PWM 控制。

| 功能 | 器件或接口 |
| --- | --- |
| MCU | ESP32-C5 |
| 存储器 | 原理图模组为 ESP32-C5-WROOM-1-N32R8，32 MB Flash、8 MB PSRAM；示例采用 16 MB Flash 布局 |
| 显示 | 2.8 英寸 ST7789 SPI LCD，240 × 320 |
| 触摸 | CST3530，I2C |
| 音频 | ES8311 编解码器和板载麦克风 |
| 传感器 | QMI8658 陀螺仪、PCF85063 RTC、SHTC3 温湿度 |
| 扩展 | CH32V003 IO 扩展、共享 SPI 的 microSD |
| 板级支持 | [公共 BSP 与兼容说明](docs/compatibility_ZH.md) |

## 📦 快速开始

1. 在 [Releases](https://github.com/waveshareteam/ESP32-C5-Touch-LCD-2.8/releases) 下载所需示例固件包。
2. 解压，通过 `python -m pip install esptool` 安装烧录工具。
3. 通过 USB 连接开发板。
4. 按固件包内的脚本和说明烧录，然后复位开发板。

出厂固件：在 [`firmware`](firmware/) 目录下载文件名带 `FactoryOnly` 的文件，可恢复开箱演示。

## 🧪 示例

### ESP-IDF

| 示例 | 主要功能 |
| --- | --- |
| [01_lvgl_demo](examples/esp-idf/01_lvgl_demo/README_ZH.md) | LVGL widgets、触摸和背光 |
| [02_sd_card](examples/esp-idf/02_sd_card/README_ZH.md) | SD 文件写入与读回校验 |
| [03_qmi8658](examples/esp-idf/03_qmi8658/README_ZH.md) | 串口加速度与角速度 |
| [04_pcf85063](examples/esp-idf/04_pcf85063/README_ZH.md) | RTC 设置与读取 |
| [05_shtc3](examples/esp-idf/05_shtc3/README_ZH.md) | 串口温湿度 |
| [06_exio](examples/esp-idf/06_exio/README_ZH.md) | 排针 EXIO6/7 与背光 PWM |
| [07_audio](examples/esp-idf/07_audio/README_ZH.md) | 录音三秒并通过扬声器回放 |
| [08_battery_voltage](examples/esp-idf/08_battery_voltage/README_ZH.md) | LCD/串口显示电池电压及 ADC |

### Arduino

| 示例 | 主要功能 |
| --- | --- |
| [01_exio](examples/arduino/examples/01_exio/README_ZH.md) | CH32V003 IO 扩展 |
| [02_I2C_qmi8658](examples/arduino/examples/02_I2C_qmi8658/README_ZH.md) | 陀螺仪数据 |
| [03_SD_Card](examples/arduino/examples/03_SD_Card/README_ZH.md) | SD 文件操作 |
| [04_I2C_pcf85063](examples/arduino/examples/04_I2C_pcf85063/README_ZH.md) | RTC |
| [05_shtc3](examples/arduino/examples/05_shtc3/README_ZH.md) | 温湿度 |
| [06_lvgl_demo](examples/arduino/examples/06_lvgl_demo/README_ZH.md) | 显示、触摸和背光 |
| [07_battery_voltage](examples/arduino/examples/07_battery_voltage/README_ZH.md) | 电池电压和 ADC 原始值 |

公共库位于 [`examples/arduino/libraries`](examples/arduino/libraries/)。Arduino 使用 LVGL 8.4，ESP-IDF 和 factory 使用 LVGL 9.5.0。

### 出厂演示

[factory 工程](examples/factory/README_ZH.md) 提供 Display Test、Settings、AIChats、Touch Test、GravityBall、Photos、Weather、SD Music、Sensor、2048、Mic Test 和 Button Test，共 12 个应用。AIChats 集成在同一桌面固件内；配网、激活和应用操作请参照[使用说明](examples/factory/USER_GUIDE_ZH.md)。

## 🛠️ 开发环境

开发框架版本和板卡选项统一定义在 [product.json](.github/product.json)。

| 开发框架 | 版本 | 示例数 |
| --- | --- | ---: |
| ESP-IDF | `v5.5.4` | 8 |
| ESP-IDF（出厂工程） | `v5.5.4` | 1 |
| Arduino-ESP32 | `3.3.12` | 7 |

按各示例 README 编译和烧录。各 IDF 工程自带 BSP 和本地驱动，使用片内 LCD DMA 缓冲，并统一管理 ES8311 的录音与播放句柄。

各基础示例与出厂工程均自带 components 目录，BSP 接口保持一致，复制完整的单个工程目录即可独立编译。出厂工程使用上表中的出厂工程版本构建。

## 📚 资料

- [原理图](hardware/schematic/)
- [机械资料（尺寸图和 3D 模型）](hardware/mechanical/)
- [产品页](https://www.waveshare.com/ESP32-C5-Touch-LCD-2.8.htm)
- [Wiki](https://docs.waveshare.net/ESP32-C5-Touch-LCD-2.8/)

## 🤝 支持

- 产品使用、硬件或订单问题：[微雪技术支持](https://docs.waveshare.net/ESP32-C5-Touch-LCD-2.8/Technical-Support/)。

## 📄 许可证

Apache License 2.0，详见 [LICENSE](LICENSE)。
