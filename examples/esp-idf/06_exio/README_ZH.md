# CH32V003 扩展 IO

[English](README.md) | [简体中文](README_ZH.md)

## 功能

每秒翻转排针上的 EXIO6/7 输出，并将背光 PWM 在关闭与全亮之间切换。

## 硬件需求

ESP32-C5-Touch-LCD-2.8 开发板和 USB 数据线。可用万用表测量排针 EXIO6/7 相对开发板 GND 的电平；只观察背光时无需额外接线。

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

串口交替显示 `EXIO6/7=0; backlight PWM=0` 与 `EXIO6/7=1; backlight PWM=255`。背光会明暗切换，但未初始化的 LCD 不显示应用界面。

## 常见问题

- 测量排针 EXIO6/7 即可，其他扩展 IO 用于复位、功放和传感器控制。保留板载 CH32V003 固件，详见[引脚用途](../../../docs/ch32v003_ZH.md)。
- 串口没有输出：检查是否使用 USB 数据线、串口是否选对，关闭其他占用串口的软件，再按 RESET；监视器波特率为 115200。
- 切换 SDK 后编译报错：确认当前环境是 **ESP-IDF v5.5.4**，执行 `idf.py fullclean` 后重新编译。本仓库使用 5.5.x 驱动接口，不使用 ESP-IDF 6.x 环境。
