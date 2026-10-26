# PCF85063 实时时钟

[English](README.md) | [简体中文](README_ZH.md)

## 功能

每次上电先为板载 RTC 设置演示时间，再每秒从芯片读取日期和时间并通过串口显示。

## 硬件需求

ESP32-C5-Touch-LCD-2.8 开发板和 USB 数据线，使用板载 RTC，无需外接时钟模块。

## 怎么运行

1. 安装并启用 [ESP-IDF v5.5.4](https://docs.espressif.com/projects/esp-idf/zh_CN/v5.5.4/esp32c5/get-started/)。使用 VS Code 时选择这个已安装的环境，再打开包含 `CMakeLists.txt` 的本示例目录；工程自动选择 ESP32-C5。
2. 使用 USB 数据线连接开发板与电脑，在**本示例目录**打开 ESP-IDF 终端，依次执行：

   ```sh
   idf.py build
   idf.py flash monitor
   ```

3. 第二条命令会烧录程序并以 **115200 波特率**打开串口日志。电脑有多个串口时，改用 `idf.py -p PORT flash monitor`，把 `PORT` 替换为设备管理器里开发板的串口；按 `Ctrl+]` 退出监视器。

每个示例自带 BSP 和本地驱动，可以单独复制。烧录示例后将运行该示例，需要恢复桌面时重新烧录[出厂固件](../../../firmware/README_ZH.md)。

## 预期现象

串口时间从 `2026-10-24 12:00:00` 开始，秒数持续增加。本示例只通过串口测试 RTC，屏幕不会点亮。

## 常见问题

- 按 RESET 后会重新设置演示日期，不是自动网络校时。需要改变初始时间时，修改 `main/main.c` 中的 `initial`，再编译烧录。
- 串口没有输出：检查是否使用 USB 数据线、串口是否选对，关闭其他占用串口的软件，再按 RESET；监视器波特率为 115200。
- 切换 SDK 后编译报错：确认当前环境是 **ESP-IDF v5.5.4**，执行 `idf.py fullclean` 后重新编译。本仓库使用 5.5.x 驱动接口，不使用 ESP-IDF 6.x 环境。
