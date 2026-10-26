# SHTC3 温湿度

[English](README.md) | [简体中文](README_ZH.md)

## 功能

每秒读取板载 SHTC3 的温度和相对湿度，检查传感器数据的 CRC 后通过串口输出。

## 硬件需求

ESP32-C5-Touch-LCD-2.8 开发板和 USB 数据线，使用板载温湿度传感器，无需 SD 卡或其他接线。

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

串口每秒显示一次温度（`C`）和相对湿度（`%RH`），例如 `25.00 C; 50.00 %RH`。本示例只初始化传感器和所需 I2C 总线，屏幕保持黑屏。

## 常见问题

- 编译出现 `esp_timer_stop_blocking` 时，是 SDK 与显示适配组件版本不匹配，不是传感器损坏。选择 v5.5.4，清理旧 build 后重编；不要手动修改下载的 `managed_components` 源码。
- 运行时出现 I2C 或 CRC 错误，先重新上电并查看串口错误。对比室温时不要遮住或弄湿传感器。
- 串口没有输出：检查是否使用 USB 数据线、串口是否选对，关闭其他占用串口的软件，再按 RESET；监视器波特率为 115200。
- 切换 SDK 后编译报错：确认当前环境是 **ESP-IDF v5.5.4**，执行 `idf.py fullclean` 后重新编译。本仓库使用 5.5.x 驱动接口，不使用 ESP-IDF 6.x 环境。
