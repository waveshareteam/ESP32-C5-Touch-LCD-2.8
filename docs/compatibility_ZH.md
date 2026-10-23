# 组件与硬件

[English](compatibility.md) | [简体中文](compatibility_ZH.md)

## 板卡与构建配置

[原理图](../hardware/schematic/ESP32-C5-Touch-LCD-2.8.pdf) 标注的模组是 ESP32-C5-WROOM-1-N32R8，带有 32 MB Flash 和 8 MB PSRAM。示例使用 16 MB Flash 布局。框架版本和 Arduino 板卡选项统一见 [product.json](../.github/product.json)。

## 公共 BSP

所有带图形界面的 IDF 示例使用 [components](../examples/esp-idf/components/esp32_c5_touch_lcd_2_8/) 中的一份 BSP；CH32V003 专用示例直接测试对应驱动。LCD 使用两块常驻的 20 行片内 DMA 缓冲，每块 9,600 字节，避免刷新时临时申请 SPI 发送缓冲。SD 文本与音乐示例通过 CONFIG_BSP_LCD_DATA_LITTLE_ENDIAN 保留字节序配置；触摸仍通过 user_data 传递公开 I2C 总线句柄。

ES8311 录音与播放共用 BSP 管理的输入输出句柄，提供 24 kHz 语音初始化、有超时的 I2S 读写、功放控制与完整音频释放接口。调用方应先停止音频任务并关闭编解码器，再释放底层资源，不应自行删除共享句柄。

## 出厂 BSP

[出厂工程](../examples/factory/README_ZH.md)使用独立重构的 BSP，通过 `board.h`、`display.h`、`touch.h` 提供接口，并使用共享音频会话组件。保留 C5 引脚及 CST3530 驱动，Display Test 通过 LVGL 判断双击，不使用 FT6336 手势寄存器。普通示例继续使用上述公共 BSP，不随出厂源码同步而改变。

## 依赖

保留 CST3530 与 SHTC3 的本地适配，因为其复位、总线和测量流程与已发布驱动并非直接等价。IDF 生成界面继续使用 LVGL 9.3，Arduino 使用 LVGL 8.4。编解码器、LVGL 适配层、QMI8658、PCF85063 和 CH32V003 继续由组件清单管理，1.x API 约束保持当前应用接口。

[兼容 CMake 文件](../examples/esp-idf/components/idf_compat.cmake) 在 IDF 6 下为保留的驱动添加 I2C 依赖，并保留 QMI8658 宏转发头文件及 GCC 15 下 Boost 的局部告警处理。不修改下载的托管组件源码；兼容组件版本发布后，可移除此处适配。

## 音频接线

GPIO24 为 BCLK，GPIO25 为 LRCK；GPIO26 接收编解码器 ASDOUT，GPIO27 向编解码器 DSDIN 发送数据，没有外部 MCLK。CH32V003 的 EXIO3 控制功放。共享 SPI 使用 GPIO6/7/8，LCD CS 为 GPIO10，SD CS 为 GPIO23。合入出厂音频接口时保留这些与原理图一致的映射。

## 固件

[FactoryOnly V1.1 镜像](../firmware/ESP32-C5-Touch-LCD-2.8_FactoryOnly_v1.1.bin) 保留原始内容，SHA-256 为 92d0f25d7b8c43900c8882e2ca9cee1ca7c8a9338504bd392567f3d1e8cdf174。本地示例编译不会覆盖此交付镜像。
