# PCF85063 实时时钟

[English](README.md) | [简体中文](README_ZH.md)

## 功能

上电时设置演示时间，每秒读取一次板载 RTC。

## 硬件需求

ESP32-C5-Touch-LCD-2.8 开发板和 USB 数据线。使用板载 RTC，无需外接时钟模块。

## 怎么运行

1. 安装 Arduino IDE，在**首选项 → 附加开发板管理器网址**中添加 `https://espressif.github.io/arduino-esp32/package_esp32_index.json`。打开开发板管理器，安装 **esp32 by Espressif Systems 3.3.12**，版本与 [product.json](../../../../.github/product.json) 一致。
2. 把**首选项 → 项目文件夹位置**设为本仓库的 `examples/arduino`，重启 IDE，让它找到附带的 [libraries](../../libraries/)；打开本示例目录里的 `.ino` 文件。
3. 在**工具**菜单选择 **ESP32C5 Dev Module**，Flash Size 选 **16 MB**，Flash Mode 选 **QIO**，PSRAM 选 **Enabled**，Partition Scheme 选 **16M Flash (3MB APP/9.9MB FATFS)**。使用 USB 数据线连接开发板并选择对应串口。
4. 点击**上传**，完成后打开**串口监视器**，设为 **115200 波特率**，再按 RESET 观察启动信息。

## 预期现象

串口时间从 2025/9/9 14:51:30 开始，每秒递增；屏幕保持黑屏，RESET 后会重新设置演示时间。

## 常见问题

- 需要改变初始日期时，修改草图里的 i2c_rtc_setTime 调用并重新上传；本示例不通过 Wi-Fi 校时。
- 提示缺少头文件或库：确认项目文件夹位置和附带的 libraries 目录，重启 Arduino IDE；不要把附带的 LVGL 8.4 换成其他大版本。
- 上传失败或串口无输出：检查 USB 数据线及工具菜单中的串口，关闭其他串口软件，确认监视器为 115200 波特率后按 RESET。
