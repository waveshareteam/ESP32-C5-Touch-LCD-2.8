# SD 卡读写与屏幕显示

[English](README.md) | [简体中文](README_ZH.md)

## 功能

写入测试文件后重新读取并逐字节校验，在 LCD 上显示结果和读回内容。先完成 SD 读写，再启动与 SD 共用 SPI 总线的屏幕。

## 硬件需求

ESP32-C5-Touch-LCD-2.8 开发板、USB 数据线和 FAT32 microSD 卡。上电前插卡，每次运行会覆盖卡根目录的 `test.txt`，本示例不会格式化 SD 卡。

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

串口依次显示 `SD card mounted`、`Write successful`、`Read successful` 和 `Write/read verification: ESP_OK`。屏幕显示 `Write / read passed`、路径 `/sdcard/test.txt` 和读回内容：

```text
ESP32-C5-Touch-LCD-2.8 SD test
```

校验完成后会卸载 SD 卡，屏幕保留结果。可以取卡在电脑中打开 `test.txt` 检查；按 RESET 重新测试。未插卡或挂载失败时，屏幕显示 `SD card not mounted`，提示插入 FAT32 卡后按 RESET 重试。

## 常见问题

- 单独出现 `cmd=5 ... command not supported` 不代表文件测试失败：CMD5 用于探测 SDIO 功能，普通存储卡可能不支持。应以之后的挂载、写入和读回校验结果为准。
- 挂载失败：重新插卡，在电脑确认文件系统为 FAT32，再按 RESET。写入或读取失败：根据屏幕显示的失败阶段查看串口错误，或换一张可正常写入的卡。
- 串口没有输出：检查是否使用 USB 数据线、串口是否选对，关闭其他占用串口的软件，再按 RESET；监视器波特率为 115200。
- 切换 SDK 后编译报错：确认当前环境是 **ESP-IDF v5.5.4**，执行 `idf.py fullclean` 后重新编译。本仓库使用 5.5.x 驱动接口，不使用 ESP-IDF 6.x 环境。
