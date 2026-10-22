# SD 文本显示

[English](README.md) | [简体中文](README_ZH.md)

## 功能

读取 SD 卡文本文件并用 LVGL 显示。

## 硬件需求

ESP32-C5-Touch-LCD-2.8。开发板和 FAT32 microSD 卡，卡根目录放置 Test.txt。

## 怎么运行

在此目录启用对应 ESP-IDF 环境后执行：

```sh
idf.py set-target esp32c5 build flash monitor
```

## 预期现象

屏幕显示文本，或提示未检测到 SD 卡、读取失败。

## 常见问题

可使用附带的 [SD 资源](../resources/sdcard/)，文件名保持 Test.txt。
