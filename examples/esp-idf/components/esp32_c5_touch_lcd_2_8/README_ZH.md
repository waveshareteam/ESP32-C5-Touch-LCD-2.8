# ESP32-C5-Touch-LCD-2.8 BSP

[English](README.md) | [简体中文](README_ZH.md)

IDF 示例共用此组件，统一提供显示、触摸、存储、传感器与 ES8311 音频。引脚以原理图为准，背光 PWM 与功放使能由 CH32V003 控制。

LCD 使用两块 20 行片内 DMA 缓冲。扬声器与麦克风初始化返回同一 BSP 管理的编解码器句柄；语音任务使用有超时的 I2S 读写接口，调用 bsp_audio_deinit() 前先停止所有音频使用方。

依赖与兼容处理见 [组件与硬件](../../../../docs/compatibility_ZH.md)。
