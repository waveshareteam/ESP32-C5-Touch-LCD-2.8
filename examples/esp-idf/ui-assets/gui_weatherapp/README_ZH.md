# 天气界面源文件

[English](README.md) | [简体中文](README_ZH.md)

## 功能

保存天气界面的 GUI Guider 工程和模拟器资源。此目录用于界面编辑，不是 ESP-IDF 固件示例。

## 硬件需求

编辑需要安装 NXP GUI Guider 的电脑；获取天气需要开发板和 Wi-Fi 网络。

## 怎么运行

用 GUI Guider 打开 `weather_app.guiguider`。开发板程序请从 [06_wifi_weather](../../06_wifi_weather/README_ZH.md) 配置、编译和烧录。

## 预期现象

编辑器展示天气界面布局，ESP-IDF 示例将获取到的天气数据填入界面。

## 常见问题

修改此目录不会自动更新固件，需要导出 UI 并整合到示例中，保持示例的 LVGL 版本。
