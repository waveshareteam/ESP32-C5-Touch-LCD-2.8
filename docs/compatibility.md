# Components and Hardware

[English](compatibility.md) | [简体中文](compatibility_ZH.md)

## Board and Build Configuration

The [schematic](../hardware/schematic/ESP32-C5-Touch-LCD-2.8.pdf) identifies an ESP32-C5-WROOM-1-N32R8 module with 32 MB Flash and 8 MB PSRAM. Existing examples retain a 16 MB Flash layout; framework versions and Arduino options are defined in [product.json](../.github/product.json).

## Project-local BSP

The eight IDF examples and [factory project](../examples/factory/) each include their own components directory, following the C5-3.5 project layout. Their BSP interfaces and drivers match; each project can be copied and built independently. Each peripheral initializer starts only its required bus, while factory uses aggregate `bsp_init()` and its local `bsp_extra` audio-session component.

Initialize peripherals from one task before starting workers. RTC, IMU, environment and battery serial reads do not start LCD, audio or Wi-Fi tasks. The CH32V003 driver's initial register reset is part of expander creation; see [pin purposes](ch32v003.md).

## Display and Storage

ST7789 and microSD share SPI2 with independent chip selects. The BSP initializes the host once; unmounting SD never frees it. Factory mounts SD before starting LCD traffic.

LCD uses two persistent 20-row internal DMA buffers (9,600 bytes each). Create the UI before calling `bsp_display_show()`, which flushes the first frame before enabling backlight. CST3530 supplies touch coordinates; factory Display Test detects double-taps in LVGL, without FT6336 gesture registers.

## Audio and Dependencies

ES8311 capture and playback share one BSP-owned IN_OUT codec. Close the codec and stop workers before `bsp_audio_deinit()`; callers must not delete borrowed handles. The standalone Audio example reuses factory Mic Test and its project-local audio-session adapter, with live level display and up to 10 seconds of touchscreen-controlled recording/playback in PSRAM; no SD card or desktop is required.

IDF uses LVGL 9.5.0 and Arduino retains LVGL 8.4. Shared Arduino board drivers reside in `examples/arduino/libraries/board_config`. Local CST3530/SHTC3 adaptations and managed 1.x driver APIs are retained. Build the basic examples and factory with ESP-IDF v5.5.4, matching the C5-3.5 projects.

The BSP pins esp_lvgl_adapter to 0.4.3~1 for the selected v5.5.4 build; its timer API check is incompatible with IDF 6.0.2. Revisit the pin when adopting a tested adapter and SDK combination.

## Firmware

Source updates and local builds do not replace the tested images in [firmware](../firmware/README.md). The complete desktop applications remain in factory; basic examples focus on individual peripherals.
