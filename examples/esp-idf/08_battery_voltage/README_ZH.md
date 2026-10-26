# 电池电压显示

[English](README.md) | [简体中文](README_ZH.md)

## 功能

对 CH32V003 ADC 的八次采样取平均，在 LCD 和串口显示估算电池电压与 ADC 原始值。

## 硬件需求

ESP32-C5-Touch-LCD-2.8 开发板、USB 数据线，以及正确连接到开发板电池座的匹配电池，接入前核对正负极。

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

屏幕和串口约每秒更新一次，显示以伏特为单位的电压及 `ADC: ... / 1023` 原始值。本示例显示电压，不显示电量百分比。

## 常见问题

- 没接电池或接触不良时，读数可能为零或异常。计算使用标称 3.3 V ADC 参考电压与 3:1 分压，需要核对精度时可用万用表比较。
- 串口没有输出：检查是否使用 USB 数据线、串口是否选对，关闭其他占用串口的软件，再按 RESET；监视器波特率为 115200。
- 切换 SDK 后编译报错：确认当前环境是 **ESP-IDF v5.5.4**，执行 `idf.py fullclean` 后重新编译。本仓库使用 5.5.x 驱动接口，不使用 ESP-IDF 6.x 环境。
