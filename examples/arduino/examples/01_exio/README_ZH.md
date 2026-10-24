# IO 扩展

[English](README.md) | [简体中文](README_ZH.md)

## 功能

每秒翻转 CH32V003 排针 EXIO6/7，并切换背光 PWM。

## 硬件需求

ESP32-C5-Touch-LCD-2.8 和 USB 数据线，可在 EXIO6/7 接万用表。

## 怎么运行

将草图目录设为 examples/arduino/，打开本草图。使用 [product.json](../../../../.github/product.json) 中的 Arduino-ESP32，选择 ESP32C5 Dev Module、16 MB Flash、QIO、启用 PSRAM、16M Flash (3MB APP/9.9MB FATFS)，然后上传。

## 预期现象

串口每秒输出交替的 EXIO6/7 状态及背光百分比。

## 常见问题

保留 CH32V003 固件。复位、功放及 SHTC3 引脚不作测试输出，参见[引脚用途](../../../../docs/ch32v003_ZH.md)。此示例不初始化 LCD。
