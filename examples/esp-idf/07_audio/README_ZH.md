# 触摸屏麦克风录音与回放

[English](README.md) | [简体中文](README_ZH.md)

## 功能

从[出厂固件 Mic Test](../../factory/components/MicTest/README_ZH.md) 拆出的独立测试示例，保留浅色界面、实时麦克风电平及 Record/Stop/Play 操作。使用 PSRAM 存储最长 10 秒的 16 kHz 单声道 PCM16，不依赖 SD 卡、网络或桌面框架。

## 硬件需求

ESP32-C5-Touch-LCD-2.8 开发板、USB 数据线，以及接到扬声器插座的匹配扬声器；使用板载麦克风和 ES8311 录音与回放。

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

1. 等待麦克风初始化完成，对着麦克风说话，电平百分比和音量条会随声音变化。
2. 点击 **Record**（录音）并说话，屏幕显示 `REC` 和录音时间，上限 **10.0 秒**。
3. 点击 **Stop**（停止）可提前结束，达到 10 秒会自动停止。
4. 点击 **Play**（播放）听录音，播放中点击 **Stop** 可中止；播放结束后可再次 Play 重听，新录音会覆盖上一段。

## 常见问题

- 尚未录音时 Play 按钮为灰色。监测电平时扬声器保持静音，避免啸叫；按 RESET 或断电后录音清除，不保存到 Flash 或 SD 卡。
- 回放无声：检查扬声器连接和录音时长是否大于零。初始化或音频读写失败时屏幕会提示错误，结合串口日志检查，处理后按 RESET 重试。
- 串口没有输出：检查是否使用 USB 数据线、串口是否选对，关闭其他占用串口的软件，再按 RESET；监视器波特率为 115200。
- 切换 SDK 后编译报错：确认当前环境是 **ESP-IDF v5.5.4**，执行 `idf.py fullclean` 后重新编译。本仓库使用 5.5.x 驱动接口，不使用 ESP-IDF 6.x 环境。
