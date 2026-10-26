# 电池电压

[English](README.md) | [简体中文](README_ZH.md)

## 功能

对 CH32V003 ADC 的八次采样取平均，显示电池估算电压及原始值。

## 硬件需求

开发板、USB 数据线以及接入电池座的电池。

## 怎么运行

工程默认选择 ESP32-C5。启用 ESP-IDF v5.5.4，在本目录依次运行 `idf.py build` 和 `idf.py -p PORT flash monitor`。工程自带所需组件，可以单独复制并编译。

## 预期现象

LCD 和串口约每秒更新电压及 ADC 值（0–1023）。

## 常见问题

使用标称 3.3 V ADC 参考及原理图的 3:1 分压，不推算电量百分比。
