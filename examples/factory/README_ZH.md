# 出厂应用工程

[English](README.md) | [简体中文](README_ZH.md)

## 功能

提供 240 × 320 竖屏 Brookesia 桌面，包含 Settings、AIChats、Sensor、Drawing、重力球、MusicPlayer、Weather、Games、Photos 和 Mic Test。小智集成在同一固件内，不需要切换启动分区。

## 硬件需求

使用 ESP32-C5-Touch-LCD-2.8 开发板，准备 USB 数据线和扬声器。相册需要 FAT32 microSD 卡，内置音乐和麦克风录音测试不需要插卡。

## 怎么运行

直接恢复软件请参照[固件烧录说明](../../firmware/README_ZH.md)。源码编译使用 ESP-IDF v5.5.4，在本目录依次执行 `idf.py set-target esp32c5`、`idf.py build`、`idf.py -p PORT flash monitor`；清单固定使用 LVGL 9.5.0。

## 预期现象

启动后进入应用桌面，MusicPlayer 可播放 SPIFFS 内置 MP3，Mic Test 可在 PSRAM 中录制最多 10 秒音频并通过扬声器回放。配网、小智激活及各应用操作请参照[使用说明](USER_GUIDE_ZH.md)。

## 常见问题

从 V1.1 升级时请完整烧录，包含 SPIFFS 和语音模型，不要只烧录应用 BIN。天气和 AIChats 需要互联网；串口监视器波特率为 2,000,000。
