# Components and hardware

English | [简体中文](compatibility_ZH.md) | [Home](../README.md)

## Framework contract

[CI configuration](../config/ci.json) selects ESP-IDF v5.5.5/v6.1 and Arduino-ESP32 3.3.12. See [validation status](validation.md); no hardware success is inferred from compilation.

The existing applications use LVGL 9 and the esp_lvgl_adapter 0.4 API. The Arduino examples keep bundled LVGL 8.4 and SensorLib. They are separate framework surfaces.

ESP-IDF 6 removes implicit peripheral dependencies from the umbrella driver component. The shared BSP explicitly requires I2C, GPIO, I2S, SPI and SDSPI drivers; the unused USB dependency is removed. Migration references: [6.0](https://docs.espressif.com/projects/esp-idf/en/v6.1/esp32c5/migration-guides/release-6.x/6.0/index.html) and [6.1](https://docs.espressif.com/projects/esp-idf/en/v6.1/esp32c5/migration-guides/release-6.x/6.1/index.html).

## Local and managed components

The 1.x CH32V003, QMI8658 and PCF85063 releases declare the old umbrella driver dependency. [idf_compat.cmake](../config/idf_compat.cmake) adds the required I2C target for IDF 6 at the product layer, without editing downloaded sources. A shared-component release that declares esp_driver_i2c itself can eventually replace this compatibility rule.

LVGL is held at 9.3.0 because the generated UI and board adapter use that API. Revisit this constraint when regenerating or validating the UI against a newer LVGL release.

For IDF 6, a local forwarding header preserves the C library's M_PI definition around the QMI8658 header, which otherwise redefines it unconditionally. Remove this wrapper after the managed driver guards its macro. The LCD uses LCD_RGB_ELEMENT_ORDER_RGB, the current name for the same RGB ordering.

Brookesia uses the managed cJSON component on IDF 6 and the built-in JSON component on IDF 5.5. Its existing esp-boost 0.3 integration remains in place. For GCC 15 and later, the deprecated variadic-comma syntax in Boost's generated Proto headers remains a warning rather than an error; other warnings retain their existing severity. Revisit this narrowly scoped exception when upgrading Boost.

| Component | Maintenance decision |
| :--- | :--- |
| esp32_c5_touch_lcd_2_8 | One shared local BSP; no equivalent published board BSP was found in the Waveshare component catalog. |
| waveshare__esp_lcd_touch_cst3530 | Keep the board's local driver: it uses 400 kHz and an I2C bus passed in user_data. The currently published implementation uses driver_data and different timing/reset behavior. These are not drop-in equivalent. |
| pedrominatel__shtc3 | Keep the locally adapted 1.4.1 driver: it sends a measurement command, waits 10 ms, then reads. The referenced upstream source uses a combined transaction. |
| bsp_extra | Local application audio glue in the music examples. |
| DrawingBoard, ball_qmi, generated, custom | Application logic and UI resources remain with their examples. |
| brookesia_core | Retain the existing 0.6.0-beta2-based local integration and upstream documentation. A framework replacement requires checking the application's LVGL 9 integration. |
| Codec, LVGL adapter, CH32V003 expander, QMI8658, PCF85063 | Continue using the ESP Component Registry dependencies. |

The expander and QMI8658 dependencies remain on the 1.x API used by this BSP. Revisit their major-version ranges only after comparing their public APIs and running the board examples. Adapter 0.4 and codec 1.5 ranges preserve the current integration contract; they are not claims of compatibility with later major APIs.

Local copies were compared with [Waveshare components](https://github.com/waveshareteam/Waveshare-ESP32-components) and [SHTC3 upstream](https://github.com/pedrominatel/esp-components/tree/4f6f61d3722f772968d00b957e5cc82e7ad1e0e0/shtc3). These links describe provenance, not additional Git dependencies. Upstream documentation and licenses remain unchanged.

## Board connections

Cross-check against the [repository schematic](../schematic/ESP32-C5-Touch-LCD-2.8.pdf). Directions below are from the ESP32-C5 side.

| Function | Connection |
| :--- | :--- |
| Shared I2C | SDA GPIO0, SCL GPIO1 |
| LCD/SD shared SPI | SCK GPIO6, MOSI GPIO7, MISO GPIO8 |
| LCD | DC GPIO9, CS GPIO10, ST7789 240 × 320 |
| SD card | CS GPIO23; SPI mode |
| Touch interrupt | GPIO5 |
| IMU interrupt | GPIO4 |
| I2S clocks | BCLK GPIO24, LRCK GPIO25; no external MCLK |
| I2S data | RX GPIO26 (codec ASDOUT), TX GPIO27 (codec DSDIN) |
| USB | D− GPIO13, D+ GPIO14 |
| CH32V003 expansion | EXIO0 touch reset; EXIO1 LCD reset; EXIO2 RTC interrupt; EXIO3 amplifier enable |
| Backlight/battery | CH32V003 PWM/ADC |

The BSP originally assigned MCU TX to codec ASDOUT and MCU RX to codec DSDIN. The shared BSP corrects this according to the schematic. Confirm audio playback on hardware before release.

The SD text/music examples retain their original little-endian LCD setting through `CONFIG_BSP_LCD_DATA_LITTLE_ENDIAN`; other examples retain the original default byte order. The touch BSP now passes the public I2C bus handle explicitly instead of relying on the private panel-IO layout.

## Firmware boundary

[FactoryApp V1.1](../Firmware/ESP32-C5-Touch-LCD-2.8-FactoryAppV1.1.bin) is a delivery artifact, not an example build output. Its SHA-256 is:

```text
92d0f25d7b8c43900c8882e2ca9cee1ca7c8a9338504bd392567f3d1e8cdf174
```

CI does not rebuild, package or replace this file.
