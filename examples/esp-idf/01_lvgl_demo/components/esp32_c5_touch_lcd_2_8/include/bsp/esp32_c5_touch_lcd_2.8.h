/* SPDX-License-Identifier: Apache-2.0 */
/* ESP32-C5-Touch-LCD-2.8 BSP. Include bsp/esp-bsp.h from applications.
 * Call bsp_init(), create the UI, then call bsp_display_show().
 * All peripheral handles are owned by the BSP. */
#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "sdkconfig.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "driver/i2s_std.h"
#include "driver/spi_master.h"
#include "esp_lcd_panel_ops.h"
#include "esp_codec_dev.h"
#include "custom_io_expander_ch32v003.h"
#include "esp_lv_adapter.h"
#include "lvgl.h"
#include "bsp/config.h"
#include "bsp/display.h"

/* Shared I2C: CH32V003, ES8311, CST3530, PCF85063A, QMI8658 and SHTC3. */
#define BSP_I2C_NUM I2C_NUM_0
#define BSP_I2C_SDA GPIO_NUM_0
#define BSP_I2C_SCL GPIO_NUM_1
#define BSP_I2C_FREQ_HZ 400000 /* Hz */

/* LCD and microSD share SPI2 clock/MOSI with separate chip selects. */
#define BSP_LCD_SPI_HOST SPI2_HOST
#define BSP_LCD_SPI_NUM BSP_LCD_SPI_HOST
#define BSP_LCD_MOSI GPIO_NUM_7
#define BSP_LCD_SCLK GPIO_NUM_6
#define BSP_LCD_PCLK BSP_LCD_SCLK
#define BSP_LCD_DC GPIO_NUM_9     /* LCD command/data select */
#define BSP_LCD_CS GPIO_NUM_10    /* LCD chip select */
#define BSP_LCD_RST GPIO_NUM_NC   /* Reset via CH32V003 EXIO1 */
#define BSP_LCD_BACKLIGHT GPIO_NUM_NC /* Backlight via CH32V003 PWM */
#define BSP_SD_MOSI BSP_LCD_MOSI
#define BSP_SD_SCK BSP_LCD_SCLK
#define BSP_SD_MISO GPIO_NUM_8
#define BSP_SD_SDCS GPIO_NUM_23   /* SD chip select */
#define BSP_SD_MOUNT_POINT "/sdcard"

/* CST3530 touch: I2C coordinates, EXIO0 reset and GPIO5 interrupt. */
#define BSP_LCD_TOUCH_RST GPIO_NUM_NC
#define BSP_LCD_TOUCH_INT GPIO_NUM_5
#define BSP_TOUCH_RST BSP_LCD_TOUCH_RST
#define BSP_TOUCH_INT BSP_LCD_TOUCH_INT

/* BOOT is the only user button; active low. */
#define BSP_BUTTON_BOOT GPIO_NUM_28
#define BSP_BUTTON_BOOT_ACTIVE_LEVEL 0

/* ES8311 full-duplex I2S: one microphone, no hardware echo reference. */
#define BSP_I2S_NUM I2S_NUM_0
#define BSP_I2S_MCLK GPIO_NUM_NC  /* Codec clock derived from BCLK */
#define BSP_I2S_BCLK GPIO_NUM_24
#define BSP_I2S_LRCLK GPIO_NUM_25
#define BSP_I2S_DOUT GPIO_NUM_26  /* MCU to codec DAC */
#define BSP_I2S_DIN GPIO_NUM_27   /* Codec ADC to MCU */
#define BSP_I2S_SCLK BSP_I2S_BCLK
#define BSP_I2S_LCLK BSP_I2S_LRCLK
#define BSP_I2S_DSIN BSP_I2S_DIN
#define BSP_POWER_AMP_IO GPIO_NUM_NC /* Amplifier enable via EXIO3 */
#define BSP_AUDIO_DMA_DESCRIPTORS 6
#define BSP_AUDIO_DMA_FRAMES 240 /* Frames per DMA descriptor */

/* CH32V003 output bitmasks, not MCU GPIO numbers. */
#define BSP_IO_EXPANDER_I2C_ADDRESS CUSTOM_IO_EXPANDER_I2C_CH32V003_ADDRESS
#define IO_LCD_TOUCH_RST IO_EXPANDER_PIN_NUM_0 /* Touch reset */
#define IO_LCD_RST IO_EXPANDER_PIN_NUM_1       /* LCD reset */
#define IO_POWER_AMP_IO IO_EXPANDER_PIN_NUM_3  /* Amplifier enable */

/* Hardware capabilities; initialization results are reported separately. */
#define BSP_CAPS_DISPLAY 1
#define BSP_CAPS_TOUCH 1
#define BSP_CAPS_BUTTONS 1
#define BSP_CAPS_AUDIO 1
#define BSP_CAPS_AUDIO_SPEAKER 1
#define BSP_CAPS_AUDIO_MIC 1
#define BSP_CAPS_SDCARD 1
#define BSP_CAPS_IMU 1
#define BSP_CAPS_CAMERA 0

#ifdef __cplusplus
extern "C" {
#endif

/* Initialize the shared I2C bus; safe to call again. */
esp_err_t bsp_i2c_init(void);
/* Release only after all attached devices have been removed. */
esp_err_t bsp_i2c_deinit(void);
/* Borrowed bus handle; NULL before initialization. */
i2c_master_bus_handle_t bsp_i2c_get_handle(void);

/* Initialize I2S. NULL selects 22050 Hz/16-bit/mono; existing channels are reused. */
esp_err_t bsp_audio_init(const i2s_std_config_t *config);
/* Initialize 24 kHz/16-bit/stereo slots for voice. */
esp_err_t bsp_audio_init_voice_24k(void);
/* Stop all audio workers before releasing codec and I2S resources. */
esp_err_t bsp_audio_deinit(void);
/* Borrowed data interface; NULL before initialization. */
const audio_codec_data_if_t *bsp_audio_get_codec_itf(void);
/* Both functions return the same BSP-owned ES8311 IN_OUT codec, or NULL on failure. */
esp_codec_dev_handle_t bsp_audio_codec_speaker_init(void);
esp_codec_dev_handle_t bsp_audio_codec_microphone_init(void);
/* Raw PCM transfers: sizes/counts in bytes, timeout in ms. Caller owns the session. */
esp_err_t bsp_audio_i2s_read(void *data, size_t size, size_t *read, uint32_t timeout_ms);
esp_err_t bsp_audio_i2s_write(void *data, size_t size, size_t *written, uint32_t timeout_ms);
/* Enable/disable the amplifier through CH32V003 EXIO3. */
esp_err_t bsp_board_pa_set(bool enabled);

typedef struct {
    esp_lv_adapter_config_t lv_adapter_cfg; /* LVGL task and timing settings */
    esp_lv_adapter_rotation_t rotation;     /* Adapter rotation enum */
    esp_lv_adapter_tear_avoid_mode_t tear_avoid_mode;
    struct { unsigned swap_xy, mirror_x, mirror_y; } touch_flags; /* Touch coordinate mapping */
} bsp_display_cfg_t;

/* Start LCD/touch/LVGL in 240x320 portrait; returns NULL on failure.
 * Uses two 20-line internal DMA buffers. Call show() after UI creation. */
lv_display_t *bsp_display_start(void);
/* Non-NULL config applies only on first start; returns NULL on failure. */
lv_display_t *bsp_display_start_with_config(const bsp_display_cfg_t *config);
/* Borrowed LVGL handles; NULL before registration. */
lv_display_t *bsp_display_get_disp_dev(void);
lv_indev_t *bsp_display_get_input_dev(void);
/* Recursive UI lock: timeout in ms, 0 tries immediately, UINT32_MAX waits indefinitely.
 * Unlock in the same task only after a successful lock. */
bool bsp_display_lock(uint32_t timeout_ms);
void bsp_display_unlock(void);
#ifdef __cplusplus
}
#endif
#include "bsp/touch.h"
#include "bsp/board.h"
