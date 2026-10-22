# SPIFFS 音乐

[English](README.md) | [简体中文](README_ZH.md)

## 功能

使用 LVGL 音乐界面播放 SPIFFS 内置 MP3。

## 硬件需求

ESP32-C5-Touch-LCD-2.8。开发板和已连接的扬声器，无需 SD 卡。

## 怎么运行

在此目录启用对应 ESP-IDF 环境后执行：

```sh
idf.py set-target esp32c5 build flash monitor
```

## 预期现象

屏幕显示音乐界面，扬声器播放内置曲目。

## 常见问题

需烧录完整工程，包括由 spiffs/ 生成的 storage 镜像。
