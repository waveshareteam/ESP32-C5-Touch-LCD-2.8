# LVGL 显示与触摸

[English](README.md) | [简体中文](README_ZH.md)

## 功能

通过屏幕上的按钮、滑块等控件，体验显示与触摸功能。

## 硬件需求

ESP32-C5-Touch-LCD-2.8 开发板和 USB 数据线，无需 SD 卡或扬声器。

## 怎么运行

1. 首次使用按 [ESP-IDF 入门说明](../../../docs/getting-started-idf_ZH.md)准备环境，再在 **`01_lvgl_demo`** 目录打开 ESP-IDF 终端。
2. 执行以下命令，第二条会烧录示例并以 **115200 波特率**打开串口监视器。

   ```sh
   idf.py build
   idf.py flash monitor
   ```

3. 点击屏幕上的按钮、拖动滑块，滑动页面体验其他控件；按 RESET 可重新开始演示。

## 预期现象

屏幕显示 LVGL 控件演示页，按钮随点击响应、滑块随手指移动；这是独立演示，不带出厂桌面。

## 常见问题

- **屏幕不亮或触摸无响应：**先按 RESET，查看串口启动信息是否报错，并按[连接检查说明](../../../docs/getting-started-idf_ZH.md#常见问题)排查。
