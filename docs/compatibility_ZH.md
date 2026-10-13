# 组件与硬件

[English](compatibility.md) | 简体中文 | [首页](../README_ZH.md)

## 框架约定

[CI 配置](../config/ci.json)使用 ESP-IDF v5.5.5/v6.1 和 Arduino-ESP32 3.3.12。全部 8 个 ESP-IDF 示例已分别在上述两个 IDF 版本编译通过，全部 6 个 Arduino 示例已在上述 core 版本编译通过。这些是已验证的框架版本，其他版本尚未完成全量验证。具体结果见[验证记录](validation_ZH.md)，编译通过不等于实物测试通过。

现有 IDF 应用使用 LVGL 9 和 esp_lvgl_adapter 0.4 API；Arduino 示例继续使用随仓库提供的 LVGL 8.4 和 SensorLib，两个开发框架分别维护。

ESP-IDF 6 不再由 driver 汇总组件隐式引入全部外设依赖，因此共享 BSP 显式声明 I2C、GPIO、I2S、SPI、SDSPI 驱动，并去除未使用的 USB 依赖。迁移说明：[6.0](https://docs.espressif.com/projects/esp-idf/en/v6.1/esp32c5/migration-guides/release-6.x/6.0/index.html)、[6.1](https://docs.espressif.com/projects/esp-idf/en/v6.1/esp32c5/migration-guides/release-6.x/6.1/index.html)。

## 本地组件与托管组件

CH32V003、QMI8658 和 PCF85063 的 1.x 组件仍声明旧 driver 汇总依赖。[idf_compat.cmake](../config/idf_compat.cmake) 在产品工程层为 IDF 6 补上 I2C 依赖，不改动下载的源码。后续共享组件版本自行声明 esp_driver_i2c 后，可移除此兼容配置。

LVGL 固定在 9.3.0，因为生成的 UI 和板级适配使用该版本 API。重新生成 UI 或验证新版 LVGL 后再调整此限制。

IDF 6 下通过本地转发头文件保护 C 标准库的 M_PI 定义，避免 QMI8658 头文件无条件重复定义。托管驱动增加宏保护后可移除这层适配。LCD 使用新版名称 LCD_RGB_ELEMENT_ORDER_RGB，RGB 排列含义保持一致。

Brookesia 在 IDF 6 下使用托管 cJSON，在 IDF 5.5 下继续使用内置 JSON 组件，并保留现有 esp-boost 0.3 集成。GCC 15 及以上版本针对 Boost 生成的 Proto 头文件中的旧可变参数逗号语法，保留警告但不升级为错误，其他警告级别不变。升级 Boost 后应重新检查并移除这一针对性兼容配置。

| 组件 | 处理说明 |
| :--- | :--- |
| esp32_c5_touch_lcd_2_8 | 合并为一份共享本地 BSP；微雪组件目录中未找到对应的已发布板级 BSP。 |
| waveshare__esp_lcd_touch_cst3530 | 保留板级驱动：通过 user_data 传入 I2C 总线，使用 400 kHz；当前发布版使用 driver_data，时序和复位行为也不同，不能直接等价替换。 |
| pedrominatel__shtc3 | 保留基于 1.4.1 的本地适配：发送测量命令、等待 10 ms 后读取；所引用上游使用合并读写事务。 |
| bsp_extra | 音乐示例中的本地音频应用适配。 |
| DrawingBoard、ball_qmi、generated、custom | 保留各示例的业务代码及 UI 资源。 |
| brookesia_core | 保留现有基于 0.6.0-beta2 的集成及上游文档；替换框架前需要重新检查 LVGL 9 适配。 |
| Codec、LVGL adapter、CH32V003 扩展、QMI8658、PCF85063 | 继续通过 ESP Component Registry 获取依赖。 |

IO 扩展和 QMI8658 保持在当前 BSP 使用的 1.x API 范围。升级主版本前需要对比公开接口并运行实物示例。Adapter 0.4、codec 1.5 的版本范围用于保留现有集成约定，不代表兼容后续主版本 API。

本地代码已与[微雪组件仓库](https://github.com/waveshareteam/Waveshare-ESP32-components)、[SHTC3 上游](https://github.com/pedrominatel/esp-components/tree/4f6f61d3722f772968d00b957e5cc82e7ad1e0e0/shtc3)对比。这些链接用于说明来源，不是新增 Git 依赖；上游文档和许可证保持原样。

## 板级连接

依据[仓库原理图](../schematic/ESP32-C5-Touch-LCD-2.8.pdf)核对，以下收发方向以 ESP32-C5 为准。

| 功能 | 连接 |
| :--- | :--- |
| 共享 I2C | SDA GPIO0，SCL GPIO1 |
| LCD/SD 共享 SPI | SCK GPIO6，MOSI GPIO7，MISO GPIO8 |
| LCD | DC GPIO9，CS GPIO10，ST7789 240 × 320 |
| SD 卡 | CS GPIO23，使用 SPI 模式 |
| 触摸中断 | GPIO5 |
| IMU 中断 | GPIO4 |
| I2S 时钟 | BCLK GPIO24，LRCK GPIO25，不使用外部 MCLK |
| I2S 数据 | RX GPIO26（codec ASDOUT），TX GPIO27（codec DSDIN） |
| USB | D− GPIO13，D+ GPIO14 |
| CH32V003 扩展 | EXIO0 触摸复位；EXIO1 LCD 复位；EXIO2 RTC 中断；EXIO3 功放使能 |
| 背光及电池 | CH32V003 PWM/ADC |

原 BSP 将 MCU TX 接到 codec ASDOUT、MCU RX 接到 codec DSDIN，共享 BSP 已按原理图纠正。发布前需要实物确认音频播放。

SD 文本和 SD 音乐示例通过 `CONFIG_BSP_LCD_DATA_LITTLE_ENDIAN` 保留原有 LCD 小端配置，其余示例保留原来的默认字节序。触摸初始化现在显式传入公开的 I2C 总线句柄，不再依靠 panel-IO 私有结构布局。

## 固件边界

[FactoryApp V1.1](../Firmware/ESP32-C5-Touch-LCD-2.8-FactoryAppV1.1.bin) 是交付固件，不是示例编译产物。SHA-256：

```text
92d0f25d7b8c43900c8882e2ca9cee1ca7c8a9338504bd392567f3d1e8cdf174
```

CI 不重新编译、打包或替换此文件。
