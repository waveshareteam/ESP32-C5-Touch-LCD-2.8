# 陀螺仪小球

[English](README.md) | [简体中文](README_ZH.md)

## 功能

读取 QMI8658 数据，控制屏幕小球移动。

## 硬件需求

ESP32-C5-Touch-LCD-2.8。开发板和 USB 数据线。

## 怎么运行

在此目录启用对应 ESP-IDF 环境后执行：

```sh
idf.py set-target esp32c5 build flash monitor
```

## 预期现象

倾斜开发板时，小球随之移动。

## 常见问题

暂无。
