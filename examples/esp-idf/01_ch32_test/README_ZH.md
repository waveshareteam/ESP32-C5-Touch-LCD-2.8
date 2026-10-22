# CH32V003 IO 测试

[English](README.md) | [简体中文](README_ZH.md)

## 功能

初始化 IO 扩展器，测试输入输出。

## 硬件需求

ESP32-C5-Touch-LCD-2.8。开发板和 USB 数据线。

## 怎么运行

在此目录启用对应 ESP-IDF 环境后执行：

```sh
idf.py set-target esp32c5 build flash monitor
```

## 预期现象

串口输出 IO 操作及输入电平。

## 常见问题

CH32V003 需保留出厂 IO 扩展固件。
