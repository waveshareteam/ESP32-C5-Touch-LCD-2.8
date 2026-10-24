# SHTC3 温湿度

[English](README.md) | [简体中文](README_ZH.md)

## 功能

读取环境温度和相对湿度，不启动显示。

## 硬件需求

开发板和 USB 数据线。

## 怎么运行

启用 [product.json](../../../.github/product.json) 中的 ESP-IDF 版本，在本目录依次运行 `idf.py set-target esp32c5`、`idf.py build` 和 `idf.py -p PORT flash monitor`。保留仓库目录结构，以便找到共享组件。

## 预期现象

串口每秒输出摄氏温度和相对湿度。

## 常见问题

请保留板载 CH32V003 固件；EXIO4/5 用于传感器，不应翻转。
