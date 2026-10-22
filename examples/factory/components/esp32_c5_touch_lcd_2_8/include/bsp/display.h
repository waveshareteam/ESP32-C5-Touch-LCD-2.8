#pragma once
#include "esp_lcd_types.h"

/* LCD color formats */
#define ESP_LCD_COLOR_FORMAT_RGB565    (1)
#define ESP_LCD_COLOR_FORMAT_RGB888    (2)

/* LCD display configuration */
#define BSP_LCD_COLOR_FORMAT        (ESP_LCD_COLOR_FORMAT_RGB565)   /*!< LCD color format */
#define BSP_LCD_BIGENDIAN           (0)                             /*!< LCD color bytes endianess */
#define BSP_LCD_BITS_PER_PIXEL      (16)                            /*!< LCD bits per pixel */
#define BSP_LCD_COLOR_SPACE         (ESP_LCD_COLOR_SPACE_RGB)       /*!< LCD color space */
#define BSP_LCD_H_RES              (240)                            /*!< LCD horizontal resolution */
#define BSP_LCD_V_RES              (320)                            /*!< LCD vertical resolution */

/* SPI LCD configuration */
#define BSP_LCD_SPI_NUM             SPI2_HOST                       /*!< LCD SPI host number */
#define BSP_LCD_PIXEL_CLOCK_HZ      (20 * 1000 * 1000)               /*!< LCD pixel clock frequency */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief BSP display configuration structure
 */
typedef struct {
    int max_transfer_sz;    /*!< Maximum transfer size, in bytes. */
} bsp_display_config_t;

/**
 * @brief Create new display panel
 * 
 * @param[in]  config    display configuration
 * @param[out] ret_panel esp_lcd panel handle
 * @param[out] ret_io    esp_lcd IO handle
 * @return
 *      - ESP_OK         On success
 *      - Else           esp_lcd failure
 */
esp_err_t bsp_display_new(const bsp_display_config_t *config, esp_lcd_panel_handle_t *ret_panel, esp_lcd_panel_io_handle_t *ret_io);

/**
 * @brief Fill the entire LCD screen with a solid color
 * @param[in] panel_handle Handle to the LCD panel
 * @param[in] color        16-bit RGB565 color value
 */
void lcd_fill_screen(esp_lcd_panel_handle_t panel_handle, uint16_t color);

/**
 * @brief Turn on display backlight
 * @note Backlight is controlled by IO expander CH32V003
 * @return
 *      - ESP_OK    On success
 *      - Others    Failure
 */
// esp_err_t bsp_display_backlight_on(void);

/**
 * @brief Turn off display backlight
 * @note Backlight is controlled by IO expander CH32V003
 * @return
 *      - ESP_OK    On success
 *      - Others    Failure
 */
esp_err_t bsp_display_backlight_off(void);
esp_err_t bsp_display_backlight_on(void);

/**
 * @brief Initialize LCD backlight brightness controller
 * @note Uses PWM function of CH32V003 IO expander
 * @return
 *      - ESP_OK    On success
 *      - Others    Failure
 */
esp_err_t bsp_display_brightness_init(void);

/**
 * @brief Set LCD backlight brightness (0-100%)
 * @note Supports PWM dimming via CH32V003 IO expander
 * @param[in] brightness_percent  Brightness percentage (0 ~ 100)
 * @return
 *      - ESP_OK    On success
 *      - Others    Failure
 */
esp_err_t bsp_display_brightness_set(int brightness_percent);


#ifdef __cplusplus
}
#endif