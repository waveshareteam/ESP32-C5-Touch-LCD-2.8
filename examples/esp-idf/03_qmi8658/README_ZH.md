# QMI8658 惯性传感器

[English](README.md) | [简体中文](README_ZH.md)

## 功能

通过共享 BSP 读取加速度和角速度。

## 硬件需求

开发板和 USB 数据线。

## 怎么运行

启用 [product.json](../../../.github/product.json) 中的 ESP-IDF 版本，在本目录依次运行 `idf.py set-target esp32c5`、`idf.py build` 和 `idf.py -p PORT flash monitor`。保留仓库目录结构，以便找到共享组件。

## 预期现象

串口每 500 ms 输出加速度（g）和角速度（度/秒）。

## 常见问题

屏幕保持关闭，移动开发板可观察读数变化。
