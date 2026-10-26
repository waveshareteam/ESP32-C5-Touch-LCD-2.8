# SD 卡识别

[English](README.md) | [简体中文](README_ZH.md)

## 功能

检查开发板能否识别 microSD 卡，在串口查看卡信息，屏幕不点亮。

## 硬件需求

ESP32-C5-Touch-LCD-2.8 开发板和 USB 数据线。FAT32 microSD 测试卡，使用前先备份内容：本 Arduino 示例在挂载失败时可能格式化卡片。

## 怎么运行

1. 按 [Arduino 入门说明](../../../../docs/getting-started-arduino_ZH.md)安装环境、设置开发板和附带库，再打开本目录的 **`03_SD_Card.ino`**。
2. 先断电插好已备份的测试卡，再连接 USB。选择开发板串口，点击**上传**，完成后打开**串口监视器**，设为 **115200 波特率**，再按 RESET。
3. 按 RESET 后等待约三秒，在串口查看卡信息。

## 预期现象

启动后串口打印一次卡信息及 `practical_size` 容量值；不显示屏幕界面，也不创建测试文件，按 RESET 可重新识别。

## 常见问题

- **没有卡信息：**重新插卡，在电脑确认 FAT32 格式，再按 RESET；启动信息只打印一次。
- **为什么没有 test.txt？**本草图只识别卡，需要文件读写和屏幕结果请用 [IDF 02_sd_card](../../../esp-idf/02_sd_card/README_ZH.md)。
