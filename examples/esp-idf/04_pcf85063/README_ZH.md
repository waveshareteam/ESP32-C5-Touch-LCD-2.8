# PCF85063 实时时钟

[English](README.md) | [简体中文](README_ZH.md)

## 功能

设置演示日期并读取硬件 RTC。

## 硬件需求

开发板和 USB 数据线。

## 怎么运行

启用 [product.json](../../../.github/product.json) 中的 ESP-IDF 版本，在本目录依次运行 `idf.py set-target esp32c5`、`idf.py build` 和 `idf.py -p PORT flash monitor`。保留仓库目录结构，以便找到共享组件。

## 预期现象

串口每秒输出一次日期和时间。

## 常见问题

每次启动都会写入演示日期；若需保留已有 RTC 日期，请移除该设置调用。
