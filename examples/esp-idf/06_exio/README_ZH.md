# CH32V003 扩展 IO

[English](README.md) | [简体中文](README_ZH.md)

## 功能

检查排针 EXIO6/7 的输出，观察背光每秒一次的明暗切换。

## 硬件需求

ESP32-C5-Touch-LCD-2.8 开发板和 USB 数据线。可用万用表测量排针 EXIO6/7 相对开发板 GND 的电平；只观察背光时无需额外接线。

## 怎么运行

1. 首次使用按 [ESP-IDF 入门说明](../../../docs/getting-started-idf_ZH.md)准备环境，再在 **`06_exio`** 目录打开 ESP-IDF 终端。
2. 执行以下命令，第二条会烧录示例并以 **115200 波特率**打开串口监视器。

   ```sh
   idf.py build
   idf.py flash monitor
   ```

3. 观察背光明暗切换；如需检查输出电平，可用万用表测量 EXIO6/7 相对开发板 GND 的电压。

## 预期现象

背光交替熄灭和点亮，不显示图案或触摸界面；串口交替输出 `EXIO6/7=0; backlight PWM=0` 与 `EXIO6/7=1; backlight PWM=255`。

## 常见问题

- **背光闪烁但没有画面：**这是正常现象，需要显示界面请运行 [01_lvgl_demo](../01_lvgl_demo/README_ZH.md)。
- **应该测哪几个引脚？**只测排针 EXIO6/7 与 GND，位置和用途见[排针说明](../../../docs/ch32v003_ZH.md)。
