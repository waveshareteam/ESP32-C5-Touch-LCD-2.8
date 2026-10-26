# LVGL 显示与触摸

[English](README.md) | [简体中文](README_ZH.md)

## 功能

查看触摸坐标，通过滑块调节屏幕亮度。

## 硬件需求

ESP32-C5-Touch-LCD-2.8 开发板和 USB 数据线。无需 SD 卡或扬声器。

## 怎么运行

1. 按 [Arduino 入门说明](../../../../docs/getting-started-arduino_ZH.md)安装环境、设置开发板和附带库，再打开本目录的 **`06_lvgl_demo.ino`**。
2. 使用 USB 数据线连接开发板。选择开发板串口，点击**上传**，完成后打开**串口监视器**，设为 **115200 波特率**，再按 RESET。
3. 触摸屏幕不同位置查看坐标，再拖动底部滑块调节亮度。

## 预期现象

LCD 显示 Display OK 与触摸坐标，触摸屏幕可更新坐标，拖动底部滑块可在 5% 到 100% 之间调节背光。

## 常见问题

- **有画面但坐标不变：**查看是否提示 `Touch: not found`，先按 RESET，仍异常时保留启动日志。
- **缺少库或 LVGL 报错：**按[附带库安装步骤](../../../../docs/getting-started-arduino_ZH.md#准备-ide)操作，不要另外替换 LVGL 版本。
