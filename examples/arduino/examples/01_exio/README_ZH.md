# 扩展 IO

[English](README.md) | [简体中文](README_ZH.md)

## 功能

每秒翻转 EXIO6/7 排针输出，并切换背光。

## 硬件需求

ESP32-C5-Touch-LCD-2.8 开发板和 USB 数据线。可选用万用表测量 EXIO6/7 相对开发板 GND 的电平。

## 怎么运行

1. 安装 Arduino IDE，在**首选项 → 附加开发板管理器网址**中添加 `https://espressif.github.io/arduino-esp32/package_esp32_index.json`。打开开发板管理器，安装 **esp32 by Espressif Systems 3.3.12**，版本与 [product.json](../../../../.github/product.json) 一致。
2. 把**首选项 → 项目文件夹位置**设为本仓库的 `examples/arduino`，重启 IDE，让它找到附带的 [libraries](../../libraries/)；打开本示例目录里的 `.ino` 文件。
3. 在**工具**菜单选择 **ESP32C5 Dev Module**，Flash Size 选 **16 MB**，Flash Mode 选 **QIO**，PSRAM 选 **Enabled**，Partition Scheme 选 **16M Flash (3MB APP/9.9MB FATFS)**。使用 USB 数据线连接开发板并选择对应串口。
4. 点击**上传**，完成后打开**串口监视器**，设为 **115200 波特率**，再按 RESET 观察启动信息。

## 预期现象

串口显示 EXIO6/7 电平及 0%/100% 背光交替变化，背光随之明暗切换，本示例不显示 LCD 界面。

## 常见问题

- 保留 CH32V003 固件，仅 EXIO6/7 作为测试输出，详见[引脚用途](../../../../docs/ch32v003_ZH.md)。
- 提示缺少头文件或库：确认项目文件夹位置和附带的 libraries 目录，重启 Arduino IDE；不要把附带的 LVGL 8.4 换成其他大版本。
- 上传失败或串口无输出：检查 USB 数据线及工具菜单中的串口，关闭其他串口软件，确认监视器为 115200 波特率后按 RESET。
