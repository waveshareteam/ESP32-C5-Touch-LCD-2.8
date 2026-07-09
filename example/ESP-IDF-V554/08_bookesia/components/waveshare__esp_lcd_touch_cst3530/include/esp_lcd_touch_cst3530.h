/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 * SPDX-FileCopyrightText: 2026 OpenAI
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief ESP LCD touch: CST3530
 */

#pragma once

#include "esp_lcd_touch.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Create a CST3530 touch driver instance
 *
 * @param io I2C panel IO handle
 * @param config Touch configuration
 * @param out_touch Output touch handle
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_ARG if any argument is invalid
 *      - ESP_ERR_NO_MEM if memory allocation fails
 *      - Other error codes returned by the underlying driver initialization
 */
esp_err_t esp_lcd_touch_new_i2c_cst3530(const esp_lcd_panel_io_handle_t io,
                                        const esp_lcd_touch_config_t *config,
                                        esp_lcd_touch_handle_t *out_touch);

#define ESP_LCD_TOUCH_IO_I2C_CST3530_ADDRESS     (0x58)

/**
 * @brief Default panel_io I2C configuration for CST3530
 *
 * @note
 * CST3530 register access requires a 32-bit command, which is equivalent to
 * sending a 4-byte register address.
 *
 * @note
 * This macro keeps the configuration compatible with the calling convention of
 * esp_lcd_panel_io_rx_param() and esp_lcd_panel_io_tx_param().
 */
#define ESP_LCD_TOUCH_IO_I2C_CST3530_CONFIG()                  \
    {                                                          \
        .dev_addr = ESP_LCD_TOUCH_IO_I2C_CST3530_ADDRESS,      \
        .control_phase_bytes = 1,                              \
        .dc_bit_offset = 0,                                    \
        .lcd_cmd_bits = 32,                                    \
        .lcd_param_bits = 8,                                   \
        .flags =                                               \
        {                                                      \
            .dc_low_on_data = 0,                               \
            .disable_control_phase = 1,                        \
        }                                                      \
    }

#ifdef __cplusplus
}
#endif