# Arduino 首次使用

[English](getting-started-arduino.md) | [简体中文](getting-started-arduino_ZH.md)

按本页完成一次 IDE 设置，之后打开各示例 README 指定的草图即可；请保留完整仓库，确保附带库没有遗漏。

## 准备 IDE

1. 安装 [Arduino IDE](https://www.arduino.cc/en/software/)，解压本仓库。
2. 在**首选项 → 附加开发板管理器网址**中添加 `https://espressif.github.io/arduino-esp32/package_esp32_index.json`。
3. 打开**开发板管理器**，搜索 **esp32**，安装 **esp32 by Espressif Systems 3.3.12**；对应版本也可在[仓库首页](../README_ZH.md)的环境表查看。
4. 将**首选项 → 项目文件夹位置**设置为解压后仓库的 `examples/arduino`，重启 IDE；其中的 `libraries` 提供所需库和 LVGL，不要再用其他版本覆盖。

如果想保留原项目文件夹位置，也可以把 `examples/arduino/libraries` 中的内容复制到原项目文件夹的 `libraries` 目录，包含 `lv_conf.h`。

## 选择开发板

打开示例的 `.ino` 文件，用 USB 数据线连接开发板，按下表设置**工具**菜单：

| 菜单 | 选项 |
| --- | --- |
| 开发板 Board | ESP32C5 Dev Module |
| Flash Size | 16 MB |
| Flash Mode | QIO |
| PSRAM | Enabled |
| Partition Scheme | 16M Flash (3MB APP/9.9MB FATFS) |
| 端口 Port | 当前连接的开发板串口 |

即使模组实际 Flash 容量更大，也请按表设置，以匹配本仓库提供的示例。

## 上传与查看结果

1. 按示例 README 准备所需扬声器、电池或 SD 卡。
2. 点击**上传**，等待完成。
3. 打开**串口监视器**，设为 **115200 波特率**，再按 **RESET**（复位），查看启动信息。
4. 根据示例的“怎么运行”和“预期现象”操作；部分示例只在串口显示数据，屏幕不点亮是正常现象。

烧录后将运行该示例；需要恢复桌面时，按[出厂固件说明](../firmware/README_ZH.md)重新烧录。

## 常见问题

| 现象 | 处理方法 |
| --- | --- |
| 没有 ESP32C5 开发板选项 | 检查指定版本的 esp32 开发板包是否安装完成。 |
| 缺少头文件或库 | 检查项目文件夹位置和附带库是否完整，重启 IDE 后重试。 |
| LVGL 编译错误 | 使用附带库及 `lv_conf.h`，检查是否从其他库目录加载了不同版本的 LVGL。 |
| 找不到串口或无法上传 | 检查 USB 数据线、串口和其他串口软件；一直连接失败时，按住 BOOT，按下并松开 RESET，再松开 BOOT 后重试。 |
| 没有信息或乱码 | 监视器设为 115200，保持打开后按 RESET。 |

仍有问题时，请保留草图名称及完整错误或串口日志，联系[技术支持](https://docs.waveshare.net/ESP32-C5-Touch-LCD-2.8/Technical-Support/)。
