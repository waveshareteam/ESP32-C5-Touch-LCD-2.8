# Shared Board Support

[English](README.md) | [简体中文](README_ZH.md)

## What It Does

Provides one C5-2.8 BSP for the eight basic examples and the factory desktop, with board, display and touch interfaces.

## Hardware Required

ESP32-C5-Touch-LCD-2.8. Pin definitions remain in `include/bsp/esp32_c5_touch_lcd_2.8.h`.

## How to Run

Include `bsp/esp-bsp.h` and call only the required initializer, such as `bsp_imu_init()` or `bsp_sdcard_mount()`. Each initializer includes its bus prerequisites; `bsp_init()` is the aggregate factory startup.

## Expected Result

Serial-only examples leave LCD, audio and network tasks unstarted. Display startup retains two internal DMA buffers, and `bsp_display_show()` enables backlight after the first frame.

## Troubleshooting

Initialize from one task before starting workers. Peripheral handles are BSP-owned; audio-session arbitration belongs to the factory APP layer. See [compatibility notes](../../../../docs/compatibility.md).
