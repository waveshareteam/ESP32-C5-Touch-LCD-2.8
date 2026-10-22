# Brookesia 桌面画板

[English](README.md) | [简体中文](README_ZH.md)

## 功能

运行 Brookesia 桌面及触摸画板应用。

## 硬件需求

ESP32-C5-Touch-LCD-2.8。开发板和 USB 数据线。

## 怎么运行

在此目录启用对应 ESP-IDF 环境后执行：

```sh
idf.py set-target esp32c5 build flash monitor
```

## 预期现象

桌面可打开画板，触摸后显示笔迹。

## 常见问题

本示例为保留的桌面和画板演示，不是独立维护的 AIChats 出厂应用。
