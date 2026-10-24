/* SPDX-License-Identifier: Apache-2.0 */
/* CST3530 touch is registered by bsp_display_start().
 * APP gestures use LVGL, not FT6336 gesture registers. */
#pragma once
#include "esp_lcd_touch.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Borrowed handle; NULL before initialization. Avoid competing with LVGL reads. */
esp_lcd_touch_handle_t bsp_touch_get_handle(void);
#ifdef __cplusplus
}
#endif
