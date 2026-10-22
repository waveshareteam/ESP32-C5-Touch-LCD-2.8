# Wi-Fi 天气

[English](README.md) | [简体中文](README_ZH.md)

## 功能

连接 Wi-Fi，并定时刷新天气数据。

## 硬件需求

ESP32-C5-Touch-LCD-2.8。开发板和可访问互联网的 Wi-Fi 网络。

## 怎么运行

在此目录启用对应 ESP-IDF 环境后执行：

```sh
idf.py set-target esp32c5 build flash monitor
```

## 预期现象

屏幕显示天气，串口输出联网和更新结果。

## 常见问题

编译前检查 components/generated/events_init.c 中的 Wi-Fi 与天气参数。
