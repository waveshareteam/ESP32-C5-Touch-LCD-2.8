<div align="center">

# ESP32-C5-Touch-LCD-2.8

**2.8 英寸 ESP32-C5 Wi-Fi 6 触摸屏开发板**

中文 | [English](./README.md) | [产品页面](https://www.waveshare.net/shop/ESP32-C5-Touch-LCD-2.8.htm) | [使用文档](https://docs.waveshare.net/ESP32-C5-Touch-LCD-2.8)

<img src="./assets/ESP32-C5-Touch-LCD-2.8.png" alt="ESP32-C5-Touch-LCD-2.8" width="560">

</div>

## 简介

ESP32-C5-Touch-LCD-2.8 是一款基于 ESP32-C5 的 2.8 英寸触摸屏开发板资料包。本仓库包含 Arduino 示例、ESP-IDF 示例、出厂固件、原理图、结构尺寸文件、SD 卡示例资源以及 GUI Guider UI 工程，方便用户快速验证硬件功能并进行二次开发。

本资料包适合以下场景：

- 快速验证 ESP32-C5、LCD、触摸、传感器、SD 卡和音频功能。
- 基于 Arduino 或 ESP-IDF 开发触摸屏应用。
- 参考 BSP、LVGL 示例和 GUI Guider 工程进行 UI 二次开发。
- 查看硬件原理图、结构尺寸和出厂固件资源。

## 硬件概览

| 项目 | 说明 |
| :--- | :--- |
| 主控 | ESP32-C5 |
| 显示屏 | 2.8 英寸 LCD，240 x 320 分辨率 |
| LCD 驱动 | ST7789 |
| 触摸芯片 | CST3530，I2C 地址 `0x58` |
| IO 扩展 | CH32V003，I2C 地址 `0x24` |
| 传感器 | QMI8658 六轴传感器、PCF85063 RTC、SHTC3 温湿度传感器 |
| 存储资源 | Micro SD 卡示例资源、SPIFFS 音频示例 |
| UI 框架 | Arduino 示例使用 LVGL v8.4.0；ESP-IDF 示例使用 LVGL v9 |

> CH32V003 用于管理部分 IO 扩展、LCD 复位、触摸复位和背光控制。CH32V003 出厂已烧录控制固件，正常使用时只需要烧录 ESP32-C5 端程序。

## 目录结构

```text
.
├── dimensions/              # 结构尺寸文件：PDF、DXF、STEP
├── example/
│   ├── Arduino-v3.3.10/     # Arduino 示例和依赖库
│   └── ESP-IDF-V554/        # ESP-IDF v5.5.4 示例
├── Firmware/                # 出厂固件
├── guider_ui/               # GUI Guider 安装包/相关 UI 资源
├── schematic/               # 原理图
├── sdcard/                  # SD 卡示例文件、图片和音乐资源
├── LICENSE
├── README.cn.md
└── README.en.md
```

## Arduino 快速开始

推荐环境：

- Arduino IDE
- `esp32 by Espressif Systems v3.3.10`
- 开发板选择：`ESP32C5 Dev Module`

步骤：

1. 在 Arduino IDE 的开发板管理器中安装 `esp32 by Espressif Systems v3.3.10`。
2. 在 `Tools` > `Board` 中选择 `ESP32C5 Dev Module`。
3. 将示例包内置库复制到 Arduino 库目录：

   ```text
   example/Arduino-v3.3.10/lib/lvgl
   example/Arduino-v3.3.10/lib/SensorLib
   ```

   Windows 默认 Arduino 库目录通常为：

   ```text
   C:\Users\<用户名>\Documents\Arduino\libraries
   ```

4. 打开 `example/Arduino-v3.3.10/example` 下的 `.ino` 示例进行编译和烧录。
5. 建议先运行 `01_exio`，确认 CH32V003 IO 扩展通信正常，再运行 LCD、触摸和传感器相关示例。

### Arduino 示例

| 示例 | 功能 |
| :--- | :--- |
| `01_exio` | CH32V003 IO 扩展测试 |
| `02_I2C_qmi8658` | QMI8658 六轴传感器测试 |
| `03_SD_Card` | SD 卡读写测试 |
| `04_I2C_pcf85063` | PCF85063 RTC 测试 |
| `05_shtc3` | SHTC3 温湿度传感器测试 |
| `06_lvgl_demo` | LCD、触摸、背光和 LVGL 基础界面测试 |

## ESP-IDF 快速开始

推荐环境：

- ESP-IDF v5.5.4
- 目标芯片：`esp32c5`

进入任意 ESP-IDF 示例目录后，可使用以下命令编译和烧录：

```powershell
idf.py set-target esp32c5
idf.py build flash monitor
```

### ESP-IDF 示例

| 示例 | 功能 |
| :--- | :--- |
| `01_ch32_test` | CH32V003 IO 扩展测试，主要查看串口日志 |
| `02_2048` | 基于 LVGL 的 2048 触摸小游戏 |
| `03_sd_lvgl` | SD 卡文本读取和 LVGL 显示 |
| `04_sd_music` | SD 卡 MP3 播放测试 |
| `05_spiff_music` | SPIFFS 内置 MP3 音乐播放器示例 |
| `06_wifi_weather` | Wi-Fi 天气示例 |
| `07_Ball_qmi` | QMI8658 姿态控制小球示例 |
| `08_bookesia` | Brookesia / 画板类触摸应用示例 |

## SD 卡资源

`sdcard/` 目录包含示例所需的测试文件、图片和音乐资源。使用 SD 卡相关示例时，建议：

- 使用 64 GB 及以下 Micro SD 卡。
- 将 SD 卡格式化为 FAT32。
- 将 `sdcard/` 目录内的文件按示例要求复制到 SD 卡根目录。
- `04_sd_music` 需要在 SD 卡根目录放置 `music` 文件夹和 `.mp3` 文件。

## 出厂固件

出厂固件位于：

```text
Firmware/ESP32-C5-Touch-LCD-2.8-FactoryAppV1.1.bin
```

该固件用于恢复或验证出厂演示功能。重新烧录前请确认串口、芯片型号和 Flash 参数设置正确。

## 二次开发建议

- 常规 UI 修改优先从应用层、LVGL 页面、GUI Guider 生成代码或示例业务逻辑开始。
- 不建议优先修改 BSP、底层驱动、引脚定义和分区表。
- 如果 LCD 或触摸异常，优先确认 CH32V003 通信是否正常。
- Arduino 示例中 `06_lvgl_demo` 已适配 ST7789 和 CST3530，通常不需要额外引入 GFX 显示库或第三方触摸库。
- ESP-IDF 示例中 BSP 已适配 LCD、触摸、背光、音频、传感器和 CH32 IO 扩展。

## 常见注意事项

- Arduino 工程需要选择 `ESP32C5 Dev Module`。
- Arduino 示例目录对应 `esp32 by Espressif Systems v3.3.10`。
- ESP-IDF 示例目录 `ESP-IDF-V554` 对应 ESP-IDF v5.5.4。
- CH32V003 出厂已烧录固件，无需单独烧录。
- 运行天气示例前，请按示例配置准备可联网 Wi-Fi 热点。

## 许可证

请查看 [LICENSE](./LICENSE)。
