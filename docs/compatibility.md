# Components and hardware

[English](compatibility.md) | [简体中文](compatibility_ZH.md)

## Board and build configuration

The [schematic](../hardware/schematic/ESP32-C5-Touch-LCD-2.8.pdf) identifies an ESP32-C5-WROOM-1-N32R8 module with 32 MB Flash and 8 MB PSRAM. The examples use a 16 MB Flash layout. Framework versions and Arduino board options are defined in [product.json](../.github/product.json).

## Shared BSP

All graphical IDF examples use one local BSP in [components](../examples/esp-idf/components/esp32_c5_touch_lcd_2_8/). The CH32V003-only example exercises its driver directly. The LCD uses two persistent 20-row internal DMA buffers (9,600 bytes each), avoiding temporary SPI TX buffers during display updates. The SD text/music examples retain their little-endian setting through CONFIG_BSP_LCD_DATA_LITTLE_ENDIAN; the touch driver retains its public I2C bus in user_data.

ES8311 playback and capture share one BSP-owned input/output codec handle. The BSP provides a 24 kHz voice initializer, bounded I2S reads/writes, amplifier control and complete audio teardown. Callers must stop their audio workers and close the codec before teardown; they must not delete the shared handle themselves.

## Factory BSP

The [factory project](../examples/factory/) has its own refactored BSP with `board.h`, `display.h` and `touch.h` interfaces, plus a shared audio-session component. It retains the C5 pinout and CST3530 touch driver; Display Test uses LVGL double-tap detection rather than FT6336 gesture registers. Ordinary examples continue using the shared BSP above and are not changed by a factory source update.

## Dependencies

Local CST3530 and SHTC3 adaptations are retained because the existing reset, bus and measurement behavior is not a drop-in match for the published drivers. LVGL remains 9.3 for generated IDF UI and 8.4 for Arduino. The codec, LVGL adapter, QMI8658, PCF85063 and CH32V003 dependencies remain in component manifests; their 1.x API constraints preserve the current application contract.

The [compatibility CMake include](../examples/esp-idf/components/idf_compat.cmake) adds I2C dependencies to retained driver releases on IDF 6 and keeps the QMI8658 macro forwarding header. It also retains the narrowly scoped GCC 15 Boost warning adjustment. Downloaded managed sources are not patched. Remove these exceptions when compatible component releases make them unnecessary.

## Audio wiring

GPIO24 is BCLK and GPIO25 is LRCK. GPIO26 receives codec ASDOUT, while GPIO27 transmits to codec DSDIN; there is no external MCLK. CH32V003 EXIO3 controls the amplifier. The shared SPI bus uses GPIO6/7/8; LCD CS is GPIO10 and SD CS is GPIO23. Board definitions retain these schematic mappings when importing the factory audio APIs.

## Firmware

The [FactoryOnly V1.1 image](../firmware/ESP32-C5-Touch-LCD-2.8_FactoryOnly_v1.1.bin) retains the original bytes. Its SHA-256 is 92d0f25d7b8c43900c8882e2ca9cee1ca7c8a9338504bd392567f3d1e8cdf174. New local example builds do not replace this delivery image.
