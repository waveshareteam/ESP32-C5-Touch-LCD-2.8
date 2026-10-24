# LVGL 显示示例

[English](README.md) | [简体中文](README_ZH.md)

## 功能

运行 LVGL widgets 演示并接入 CST3530 触摸，首帧完成后再开启背光。

## 硬件需求

开发板和 USB 数据线。

## 怎么运行

启用 [product.json](../../../.github/product.json) 中的 ESP-IDF 版本，在本目录依次运行 `idf.py set-target esp32c5`、`idf.py build` 和 `idf.py -p PORT flash monitor`。保留仓库目录结构，以便找到共享组件。

## 预期现象

屏幕显示 widgets 页面，触摸可操作控件。

## 常见问题

此示例只启动显示和触摸，无需初始化传感器、SD 或音频。
