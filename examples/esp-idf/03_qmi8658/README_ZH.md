# QMI8658 惯性传感器

[English](README.md) | [简体中文](README_ZH.md)

## 功能

通过工程内 BSP 读取加速度和角速度。

## 硬件需求

开发板和 USB 数据线。

## 怎么运行

工程默认选择 ESP32-C5。启用 ESP-IDF v5.5.4，在本目录依次运行 `idf.py build` 和 `idf.py -p PORT flash monitor`。工程自带所需组件，可以单独复制并编译。

## 预期现象

串口每 500 ms 输出加速度（g）和角速度（度/秒）。

## 常见问题

屏幕保持关闭，移动开发板可观察读数变化。
