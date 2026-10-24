# 电池电压

[English](README.md) | [简体中文](README_ZH.md)

## 功能

对 CH32V003 ADC 的八次采样取平均，显示电池估算电压及原始值。

## 硬件需求

开发板、USB 数据线以及接入电池座的电池。

## 怎么运行

启用 [product.json](../../../.github/product.json) 中的 ESP-IDF 版本，在本目录依次运行 `idf.py set-target esp32c5`、`idf.py build` 和 `idf.py -p PORT flash monitor`。保留仓库目录结构，以便找到共享组件。

## 预期现象

LCD 和串口约每秒更新电压及 ADC 值（0–1023）。

## 常见问题

使用标称 3.3 V ADC 参考及原理图的 3:1 分压，不推算电量百分比；参见 [ADC 说明](../../../docs/ch32v003_ZH.md)。
