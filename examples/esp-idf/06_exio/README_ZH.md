# CH32V003 扩展 IO

[English](README.md) | [简体中文](README_ZH.md)

## 功能

每秒翻转排针 EXIO6/7，并切换背光 PWM。

## 硬件需求

开发板和 USB 数据线，可在 EXIO6/7 接入万用表。

## 怎么运行

启用 [product.json](../../../.github/product.json) 中的 ESP-IDF 版本，在本目录依次运行 `idf.py set-target esp32c5`、`idf.py build` 和 `idf.py -p PORT flash monitor`。保留仓库目录结构，以便找到共享组件。

## 预期现象

EXIO6/7 交替输出高低电平，串口显示输出状态和 PWM 值。

## 常见问题

LCD 不初始化。仅 EXIO6/7 作为测试输出，参见[引脚用途说明](../../../docs/ch32v003_ZH.md)。
