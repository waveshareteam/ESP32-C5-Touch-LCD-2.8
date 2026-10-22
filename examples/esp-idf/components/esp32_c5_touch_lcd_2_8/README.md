# ESP32-C5-Touch-LCD-2.8 BSP

[English](README.md) | [简体中文](README_ZH.md)

One shared board component provides display, touch, storage, sensors and ES8311 audio for the IDF examples. Pins follow the board schematic; the CH32V003 controls backlight PWM and amplifier enable.

The LCD uses two 20-row internal DMA buffers. Speaker and microphone initialization return the same BSP-owned codec handle; use the bounded I2S transfer APIs for voice tasks and stop all users before calling bsp_audio_deinit().

Dependency and compatibility decisions are described in [Components and hardware](../../../../docs/compatibility.md).
