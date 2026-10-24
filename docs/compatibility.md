# Components and Hardware

[English](compatibility.md) | [简体中文](compatibility_ZH.md)

## Board and Build Configuration

The [schematic](../hardware/schematic/ESP32-C5-Touch-LCD-2.8.pdf) identifies an ESP32-C5-WROOM-1-N32R8 module with 32 MB Flash and 8 MB PSRAM. Existing examples retain a 16 MB Flash layout; framework versions and Arduino options are defined in [product.json](../.github/product.json).

## Shared BSP

The eight IDF examples and [factory project](../examples/factory/) use one [BSP](../examples/esp-idf/components/esp32_c5_touch_lcd_2_8/). Each peripheral initializer starts its required bus, without calling aggregate `bsp_init()`. Factory startup uses that aggregate function; APP audio sessions stay in the factory's `bsp_extra` component.

Initialize peripherals from one task before starting workers. RTC, IMU, environment and battery serial reads do not start LCD, audio or Wi-Fi tasks. The CH32V003 driver's initial register reset is part of expander creation; see [pin purposes](ch32v003.md).

## Display and Storage

ST7789 and microSD share SPI2 with independent chip selects. The BSP initializes the host once; unmounting SD never frees it. Factory mounts SD before starting LCD traffic.

LCD uses two persistent 20-row internal DMA buffers (9,600 bytes each). Create the UI before calling `bsp_display_show()`, which flushes the first frame before enabling backlight. CST3530 supplies touch coordinates; factory Display Test detects double-taps in LVGL, without FT6336 gesture registers.

## Audio and Dependencies

ES8311 capture and playback share one BSP-owned IN_OUT codec. Close the codec and stop workers before `bsp_audio_deinit()`; callers must not delete borrowed handles. Basic Audio records three seconds into PSRAM and plays once; no SD card or desktop is required.

IDF uses LVGL 9.5.0 and Arduino retains LVGL 8.4. Shared Arduino board drivers reside in `examples/arduino/libraries/board_config`. Local CST3530/SHTC3 adaptations and managed 1.x driver APIs are retained. The [IDF compatibility include](../examples/esp-idf/components/idf_compat.cmake) adds missing IDF 6 I2C dependencies and the QMI8658 macro wrapper without editing downloaded components.

## Firmware

Source updates and local builds do not replace the tested images in [firmware](../firmware/README.md). The complete desktop applications remain in factory; basic examples focus on individual peripherals.
