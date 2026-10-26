# PCF85063 实时时钟

[English](README.md) | [简体中文](README_ZH.md)

## 功能

设置演示日期并读取硬件 RTC。

## 硬件需求

开发板和 USB 数据线。

## 怎么运行

工程默认选择 ESP32-C5。启用 ESP-IDF v5.5.4，在本目录依次运行 `idf.py build` 和 `idf.py -p PORT flash monitor`。工程自带所需组件，可以单独复制并编译。

## 预期现象

串口每秒输出一次日期和时间。

## 常见问题

每次启动都会写入演示日期；若需保留已有 RTC 日期，请移除该设置调用。
