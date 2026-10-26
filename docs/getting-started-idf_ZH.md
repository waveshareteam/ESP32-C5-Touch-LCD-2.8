# ESP-IDF 首次使用

[English](getting-started-idf.md) | [简体中文](getting-started-idf_ZH.md)

首次使用时按本页准备电脑，之后根据各示例 README 操作即可；测试板载外设不需要修改引脚，也不需要另接同类传感器。

## 准备环境

1. 下载并解压整个仓库，不要在 ZIP 压缩包里直接编译。
2. 按官方说明安装 [ESP-IDF v5.5.4（ESP32-C5）](https://docs.espressif.com/projects/esp-idf/zh_CN/v5.5.4/esp32c5/get-started/)。使用 VS Code 时安装 Espressif 的 ESP-IDF 扩展，选择已安装的 v5.5.4 环境；仓库使用的版本见[仓库首页](../README_ZH.md)的环境表。
3. 打开要运行的单个示例目录，例如 `examples/esp-idf/01_lvgl_demo`，不要打开仓库根目录或 `components`；目录中应有 `CMakeLists.txt`，工程会自动选择 **ESP32-C5**。
4. 在 VS Code 命令面板执行 **ESP-IDF: Open ESP-IDF Terminal**；也可打开已安装的 ESP-IDF 终端，再进入示例目录，后续命令都在这个终端运行。

## 连接与烧录

1. 用 USB **数据线**连接开发板；在 Windows 的**设备管理器 → 端口**中查看开发板串口，不确定时可通过拔插对比确认。
2. 按示例要求准备硬件，再执行 `idf.py build` 和 `idf.py flash monitor`；第一条编译，第二条烧录并显示串口信息。
3. 电脑连接多个串口时，使用 `idf.py -p PORT flash monitor`，把 `PORT` 替换为开发板串口，先关闭其他占用该串口的软件。
4. 按 **RESET**（复位）可重新运行示例；按 **Ctrl+]** 退出监视器，需要再次查看时，在原示例目录执行 `idf.py monitor`。

八个基础示例使用 **115200 波特率**，出厂工程使用 **2,000,000 波特率**；`idf.py monitor` 会自动选择工程的波特率，使用其他串口软件时请手动设置。

## 烧录后看哪里

根据各示例 README 的“预期现象”观察结果；部分示例只通过串口显示数据，屏幕不点亮是正常现象，不能仅凭黑屏判断烧录失败。

烧录基础示例后将运行该示例；要恢复桌面，请按[出厂固件说明](../firmware/README_ZH.md)重新烧录出厂镜像。

## 常见问题

| 现象 | 处理方法 |
| --- | --- |
| 提示找不到 `idf.py` | 使用已启用 v5.5.4 的 ESP-IDF 终端，不要直接使用普通终端。 |
| 找不到串口 | 换一根确定可传数据的 USB 线或 USB 接口，驱动与连接方法见[产品 Wiki](https://docs.waveshare.net/ESP32-C5-Touch-LCD-2.8/)。 |
| 无法打开串口 | 关闭其他串口监视器，确认开发板串口后重试。 |
| 一直停在连接中 | 按住 BOOT，按下并松开 RESET，再松开 BOOT 后重试烧录；烧录结束后按 RESET 运行。 |
| 串口乱码或没有信息 | 核对波特率，打开监视器后按 RESET；只在启动时打印的示例不会持续重复输出。 |
| 更换 IDF 版本后编译失败 | 选择 v5.5.4，执行 `idf.py fullclean` 后再 `idf.py build`，不要沿用 IDF 6.x 的旧构建。 |

仍有问题时，请保留示例名称和完整串口日志，联系[技术支持](https://docs.waveshare.net/ESP32-C5-Touch-LCD-2.8/Technical-Support/)。
