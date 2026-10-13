<div align="center">

<h1>ESP32-C5-Touch-LCD-2.8</h1>
<p><strong>2.8 英寸 ESP32-C5 Wi-Fi 6 触摸屏开发板</strong></p>
<p><a href="LICENSE"><img src="https://img.shields.io/github/license/waveshareteam/ESP32-C5-Touch-LCD-2.8" alt="License"></a></p>
<p><a href="README.md">English</a> | 简体中文</p>
<p>
<a href="https://www.waveshare.net/shop/ESP32-C5-Touch-LCD-2.8.htm">🌐 产品</a> |
<a href="https://docs.waveshare.net/ESP32-C5-Touch-LCD-2.8">📚 文档</a> |
<a href="Firmware/">📦 固件</a> |
<a href="examples/esp-idf/">🧩 ESP-IDF</a> |
<a href="examples/arduino/">🔧 Arduino</a>
</p>
<img src="assets/ESP32-C5-Touch-LCD-2.8.png" alt="ESP32-C5-Touch-LCD-2.8" width="560">

</div>

---

## ✨ 产品概述

本仓库提供 LCD、触摸、传感器、SD 卡和音频等功能的示例与配套资料。CH32V003 用于 IO 扩展、屏幕及触摸复位、背光控制，其固件已在出厂时烧录。

| 硬件 | 说明 |
| :--- | :--- |
| 主控 | ESP32-C5 |
| 显示屏 | 2.8 英寸 ST7789 LCD，240 × 320 |
| 触摸 | CST3530，I2C 地址 `0x58` |
| IO 扩展 | CH32V003，I2C 地址 `0x24` |
| 传感器 | QMI8658 IMU、PCF85063 RTC、SHTC3 温湿度传感器 |
| 音频 | ES8311 编解码器 |
| UI | ESP-IDF 使用 LVGL 9；Arduino 使用随仓库提供的 LVGL 8.4 |

修改 BSP 前，请查看[硬件与组件兼容性说明](docs/compatibility_ZH.md)。

## 🧪 编译示例

CI 配置覆盖全部 8 个 ESP-IDF 工程的 **v5.5.5、v6.1** 编译，以及全部 6 个 Arduino 示例的 **Arduino-ESP32 3.3.12** 编译。配置了覆盖范围不代表已全部通过编译或实物测试，请结合[验证记录](docs/validation_ZH.md)及当前提交的 Actions 结果判断。

### ESP-IDF

先激活相应版本的 ESP-IDF 环境，然后在仓库根目录运行：

```sh
python scripts/build_example.py esp-idf examples/esp-idf/02_2048
```

脚本根据工程默认配置，在 `build/<idf-version>/sdkconfig` 中生成独立配置，不修改已提交的配置文件。也可以进入示例目录手动编译、烧录：

```sh
idf.py set-target esp32c5
idf.py build flash monitor
```

| 示例 | 功能 |
| :--- | :--- |
| [01_ch32_test](examples/esp-idf/01_ch32_test/) | CH32V003 IO 扩展测试 |
| [02_2048](examples/esp-idf/02_2048/) | LVGL 触摸小游戏 |
| [03_sd_lvgl](examples/esp-idf/03_sd_lvgl/) | 读取 SD 卡文本并显示 |
| [04_sd_music](examples/esp-idf/04_sd_music/) | 播放 SD 卡中的 MP3 |
| [05_spiff_music](examples/esp-idf/05_spiff_music/) | SPIFFS 音乐播放器 |
| [06_wifi_weather](examples/esp-idf/06_wifi_weather/) | Wi-Fi 天气显示 |
| [07_Ball_qmi](examples/esp-idf/07_Ball_qmi/) | IMU 控制小球 |
| [08_bookesia](examples/esp-idf/08_bookesia/) | Brookesia 触摸应用及画板 |

### Arduino

安装 Arduino-ESP32 3.3.12，开发板选择 **ESP32C5 Dev Module**，设置 **16 MB Flash**、**QIO**、**启用 PSRAM**，分区选择 **16M Flash (3MB APP/9.9MB FATFS)**。使用本仓库附带的 LVGL 和 SensorLib 版本。

安装 Arduino CLI 后运行：

```sh
arduino-cli core update-index --additional-urls https://espressif.github.io/arduino-esp32/package_esp32_index.json
arduino-cli core install esp32:esp32@3.3.12 --additional-urls https://espressif.github.io/arduino-esp32/package_esp32_index.json
python scripts/build_example.py arduino examples/arduino/06_lvgl_demo
```

使用 Arduino IDE 时，将 [libraries](examples/arduino/libraries/) 内的库复制到草图目录下的 libraries 文件夹。LVGL 示例还需将 [lv_conf.h](examples/arduino/06_lvgl_demo/lv_conf.h) 放到已安装的 `lvgl` 文件夹旁边。CLI 编译脚本会自动指定该配置。

| 示例 | 功能 |
| :--- | :--- |
| [01_exio](examples/arduino/01_exio/) | CH32V003 IO 扩展 |
| [02_I2C_qmi8658](examples/arduino/02_I2C_qmi8658/) | IMU |
| [03_SD_Card](examples/arduino/03_SD_Card/) | SD 卡读写 |
| [04_I2C_pcf85063](examples/arduino/04_I2C_pcf85063/) | RTC |
| [05_shtc3](examples/arduino/05_shtc3/) | 温湿度 |
| [06_lvgl_demo](examples/arduino/06_lvgl_demo/) | LCD、触摸、背光和 LVGL |

## 📂 配套资料

| 目录 | 内容 |
| :--- | :--- |
| [examples/esp-idf](examples/esp-idf/) | IDF 工程及共享板级组件 |
| [examples/arduino](examples/arduino/) | Arduino 示例及配套库 |
| [schematic](schematic/ESP32-C5-Touch-LCD-2.8.pdf) | 原理图 |
| [dimensions](dimensions/) | [PDF](dimensions/ESP32-C5-Touch-LCD-2.8.pdf)、[DXF](dimensions/ESP32-C5-Touch-LCD-2.8.dxf)、[STEP](dimensions/ESP32-C5-Touch-LCD-2.8.stp) |
| [guider_ui](guider_ui/) | GUI Guider 工程资源 |
| [sdcard](sdcard/) | 测试文件、图片和音乐 |
| [Firmware](Firmware/) | 出厂固件 |

原 `example/ESP-IDF-V554`、`example/Arduino-v3.3.10` 中的示例已迁移到上述按开发框架划分的目录。

SD 卡示例建议使用 64 GB 或以下的 FAT32 卡，将需要的 [sdcard 资源](sdcard/) 复制到卡根目录。SD 音乐示例需要根目录中的 `music` 文件夹及 MP3 文件。运行天气示例前请配置 Wi-Fi。

## 📦 出厂固件

[FactoryApp V1.1](Firmware/ESP32-C5-Touch-LCD-2.8-FactoryAppV1.1.bin) 用于恢复出厂演示。烧录方法请查看[产品文档](https://docs.waveshare.net/ESP32-C5-Touch-LCD-2.8)。

出厂固件与示例 CI 分开维护，本次仓库整理不重新生成出厂固件。

## 🤝 开发与支持

- [参与贡献](CONTRIBUTING_ZH.md)
- [技术支持](SUPPORT_ZH.md)
- [CI 行为与本地检查](docs/ci_ZH.md)
- [组件与硬件](docs/compatibility_ZH.md)
- [验证记录](docs/validation_ZH.md)

## 📄 开源协议

第一方代码使用 [Apache-2.0](LICENSE) 协议。第三方组件保留各自的许可证及版权声明。
