# SHTC3 温湿度

[English](README.md) | [简体中文](README_ZH.md)

## 功能

每秒读取板载 SHTC3 的温度和相对湿度。

## 硬件需求

ESP32-C5-Touch-LCD-2.8 开发板和 USB 数据线。使用板载传感器，无需外接模块。

## 怎么运行

1. 安装 Arduino IDE，在**首选项 → 附加开发板管理器网址**中添加 `https://espressif.github.io/arduino-esp32/package_esp32_index.json`。打开开发板管理器，安装 **esp32 by Espressif Systems 3.3.12**，版本与 [product.json](../../../../.github/product.json) 一致。
2. 把**首选项 → 项目文件夹位置**设为本仓库的 `examples/arduino`，重启 IDE，让它找到附带的 [libraries](../../libraries/)；打开本示例目录里的 `.ino` 文件。
3. 在**工具**菜单选择 **ESP32C5 Dev Module**，Flash Size 选 **16 MB**，Flash Mode 选 **QIO**，PSRAM 选 **Enabled**，Partition Scheme 选 **16M Flash (3MB APP/9.9MB FATFS)**。使用 USB 数据线连接开发板并选择对应串口。
4. 点击**上传**，完成后打开**串口监视器**，设为 **115200 波特率**，再按 RESET 观察启动信息。

## 预期现象

串口显示 Temperature 温度（C）及 Humidity 湿度（百分比），屏幕保持黑屏。

## 常见问题

- 出现未找到传感器或读取失败时，检查供电、重新上电并查看启动日志；测量时不要弄湿或遮住传感器。
- 提示缺少头文件或库：确认项目文件夹位置和附带的 libraries 目录，重启 Arduino IDE；不要把附带的 LVGL 8.4 换成其他大版本。
- 上传失败或串口无输出：检查 USB 数据线及工具菜单中的串口，关闭其他串口软件，确认监视器为 115200 波特率后按 RESET。
