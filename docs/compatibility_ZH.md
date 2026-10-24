# 组件与硬件

[English](compatibility.md) | [简体中文](compatibility_ZH.md)

## 板卡与构建配置

[原理图](../hardware/schematic/ESP32-C5-Touch-LCD-2.8.pdf) 标注 ESP32-C5-WROOM-1-N32R8 模组，32 MB Flash、8 MB PSRAM。现有示例保留 16 MB Flash 分区，框架版本和 Arduino 选项见 [product.json](../.github/product.json)。

## 工程内 BSP

八个 IDF 示例与 [factory 工程](../examples/factory/README_ZH.md)均按照 C5-3.5 的目录结构，自带 components 目录。各工程的 BSP 接口及驱动保持一致，完整复制单个工程即可独立编译。外设独立初始化接口只启动所需总线；factory 使用整体启动入口 `bsp_init()`，音频会话保留在其本地 `bsp_extra` 组件中。

请在单一任务完成外设初始化，再启动工作任务。RTC、IMU、温湿度及电池串口读取不会启动 LCD、音频或 Wi-Fi 任务。CH32V003 驱动创建时会复位其 IO 寄存器，参见[引脚用途](ch32v003_ZH.md)。

## 显示与存储

ST7789 与 microSD 共用 SPI2，使用独立片选。BSP 只初始化一次总线，卸载 SD 不释放该总线；factory 在 LCD 开始通信前先挂载 SD。

LCD 使用两块常驻的 20 行片内 DMA 缓冲，每块 9,600 字节。先创建界面，再调用 `bsp_display_show()` 刷新首帧并开启背光。CST3530 提供触摸坐标，factory 的 Display Test 使用 LVGL 双击检测，不使用 FT6336 手势寄存器。

## 音频与依赖

ES8311 录音与播放共用 BSP 所有的 IN_OUT 句柄。调用 `bsp_audio_deinit()` 前应关闭 codec 并停止工作任务，不要删除借用的句柄。基础音频示例录音三秒到 PSRAM 后回放一次，无需 SD 卡或桌面。

IDF 使用 LVGL 9.5.0，Arduino 保留 LVGL 8.4；Arduino 共享驱动位于 `examples/arduino/libraries/board_config`。保留本地 CST3530/SHTC3 适配及托管驱动 1.x 接口。基础示例和 factory 均使用 ESP-IDF v5.5.4，与 C5-3.5 工程保持一致。

## 固件

源码更新和本地构建不会替换 [firmware](../firmware/README_ZH.md) 中已实测的镜像。完整桌面应用保留在 factory，基础示例只演示对应外设。
