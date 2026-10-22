# SD 卡

[English](README.md) | [简体中文](README_ZH.md)

## 功能

挂载 SD 卡并测试文件操作。

## 硬件需求

ESP32-C5-Touch-LCD-2.8。开发板和 FAT32 microSD 卡。

## 怎么运行

安装 [product.json](../../../../.github/product.json) 指定的 Arduino-ESP32，将项目文件夹位置设为 examples/arduino/，然后打开草图上传。选择 ESP32C5 Dev Module、16 MB Flash、QIO、启用 PSRAM，分区选择 16M Flash (3MB APP/9.9MB FATFS)。

## 预期现象

串口显示挂载和文件操作结果。

## 常见问题

文件测试会写入 SD 卡，请使用测试卡。
