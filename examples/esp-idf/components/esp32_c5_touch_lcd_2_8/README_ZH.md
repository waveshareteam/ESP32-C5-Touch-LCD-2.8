# 共享板级支持

[English](README.md) | [简体中文](README_ZH.md)

## 功能

八个基础示例与出厂桌面共用一份 C5-2.8 BSP，提供 board、display 和 touch 接口。

## 硬件需求

ESP32-C5-Touch-LCD-2.8，引脚定义保留在 `include/bsp/esp32_c5_touch_lcd_2.8.h`。

## 怎么运行

包含 `bsp/esp-bsp.h`，只调用需要的初始化接口，例如 `bsp_imu_init()` 或 `bsp_sdcard_mount()`，接口会初始化所需总线；`bsp_init()` 用于出厂固件整体启动。

## 预期现象

仅串口输出的示例不启动 LCD、音频及网络任务。显示启动保留两块片内 DMA 缓冲，`bsp_display_show()` 在首帧完成后开启背光。

## 常见问题

在单一任务中完成初始化后再启动工作任务。外设句柄归 BSP 所有，音频会话仲裁由出厂 APP 层负责，参见[兼容说明](../../../../docs/compatibility_ZH.md)。
