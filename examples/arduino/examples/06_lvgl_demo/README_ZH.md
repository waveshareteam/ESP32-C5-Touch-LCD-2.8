# LVGL 显示

[English](README.md) | [简体中文](README_ZH.md)

## 功能

使用附带的 LVGL 8.4 测试显示、触摸和背光。

## 硬件需求

ESP32-C5-Touch-LCD-2.8。开发板和 USB 数据线。

## 怎么运行

安装 [product.json](../../../../.github/product.json) 指定的 Arduino-ESP32，将项目文件夹位置设为 examples/arduino/，然后打开草图上传。选择 ESP32C5 Dev Module、16 MB Flash、QIO、启用 PSRAM，分区选择 16M Flash (3MB APP/9.9MB FATFS)。

## 预期现象

界面显示触摸信息，并可调节背光。

## 常见问题

请使用附带的 LVGL 和 libraries/lv_conf.h，不要混用 LVGL 大版本。
