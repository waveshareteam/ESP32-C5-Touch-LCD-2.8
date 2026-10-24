# 电池电压

[English](README.md) | [简体中文](README_ZH.md)

## 功能

对 CH32V003 ADC 八次采样取平均，在 LCD 和串口显示电池电压及原始值。

## 硬件需求

ESP32-C5-Touch-LCD-2.8、USB 数据线以及接入电池座的电池。

## 怎么运行

将草图目录设为 examples/arduino/，打开本草图。使用 [product.json](../../../../.github/product.json) 中的 Arduino-ESP32，选择 ESP32C5 Dev Module、16 MB Flash、QIO、启用 PSRAM、16M Flash (3MB APP/9.9MB FATFS)，然后上传。

## 预期现象

电池电压（伏特）及 ADC 值（0–1023）约每秒更新一次。

## 常见问题

计算使用标称 3.3 V 参考和 3:1 分压，不推算电量百分比。请保留随附的 LVGL 8.4 配置，参见 [ADC 说明](../../../../docs/ch32v003_ZH.md)。
