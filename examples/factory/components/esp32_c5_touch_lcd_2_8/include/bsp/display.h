/* SPDX-License-Identifier: Apache-2.0 */
/* ST7789 display and CH32V003 PWM backlight control. */
#pragma once
#include "esp_lcd_types.h"
#include "esp_err.h"
#define ESP_LCD_COLOR_FORMAT_RGB565 1
#define BSP_LCD_COLOR_FORMAT ESP_LCD_COLOR_FORMAT_RGB565
#define BSP_LCD_BIGENDIAN 0
#define BSP_LCD_BITS_PER_PIXEL 16
#define BSP_LCD_COLOR_SPACE LCD_RGB_ELEMENT_ORDER_RGB
#define BSP_LCD_H_RES 240 /* Portrait width, pixels */
#define BSP_LCD_V_RES 320 /* Portrait height, pixels */
#define BSP_LCD_PIXEL_CLOCK_HZ (80 * 1000 * 1000) /* SPI clock, Hz */
#ifdef __cplusplus
extern "C" {
#endif

typedef struct { int max_transfer_sz; /* Reserved; currently ignored */ } bsp_display_config_t;

/* Create/reuse BSP-owned panel and IO handles; does not start LVGL.
 * config is currently ignored; panel/io must be non-NULL output pointers. */
esp_err_t bsp_display_new(const bsp_display_config_t *config, esp_lcd_panel_handle_t *panel, esp_lcd_panel_io_handle_t *io);

/* Initialize backlight control; keep output off until show() succeeds. */
esp_err_t bsp_display_brightness_init(void);
/* Cache target brightness (0..100); apply only after the first show(). */
esp_err_t bsp_display_brightness_set(int percent);
/* Return the cached target, not measured PWM output. */
int bsp_display_brightness_get(void);
/* Set target brightness to 0 or 100; on() does not restore the previous level. */
esp_err_t bsp_display_backlight_off(void);
esp_err_t bsp_display_backlight_on(void);
/* After UI creation, flush the first frame and drain SPI before enabling backlight. */
esp_err_t bsp_display_show(void);
#ifdef __cplusplus
}
#endif
