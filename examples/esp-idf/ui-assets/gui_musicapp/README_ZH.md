# 音乐界面源文件

[English](README.md) | [简体中文](README_ZH.md)

## 功能

保存音乐界面的 GUI Guider 工程和模拟器资源。此目录用于界面编辑，不是 ESP-IDF 固件示例。

## 硬件需求

编辑需要安装 NXP GUI Guider 的电脑；播放音频需要开发板和扬声器。

## 怎么运行

用 GUI Guider 打开此目录中的 `.guiguider` 工程。开发板程序请从 [05_spiff_music](../../05_spiff_music/README_ZH.md) 编译和烧录。

## 预期现象

编辑器展示音乐控制界面，ESP-IDF 示例在开发板上播放内置音频。

## 常见问题

模拟器与开发板使用独立的 LVGL 配置，导出修改后需要明确更新示例中的生成界面文件。
