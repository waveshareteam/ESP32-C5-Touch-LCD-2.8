
#include <stdio.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_io_interface.h"
#include "esp_lcd_touch.h"
#include "esp_lcd_touch_cst3530.h"

static const char *TAG = "cst3530_test_app";

/* Function:
 * Board-level hardware configuration
 * Logic:
 * Adjust these GPIO numbers according to the target board schematic before use.
 */
#define TEST_I2C_NUM                     0
#define TEST_I2C_SCL_GPIO                3
#define TEST_I2C_SDA_GPIO                1
#define TEST_I2C_CLK_HZ                  400000

#define TEST_TOUCH_RST_GPIO              2
#define TEST_TOUCH_INT_GPIO              4

#define TEST_TOUCH_X_MAX                 240
#define TEST_TOUCH_Y_MAX                 320

/* Function:
 * Application context used by the test app
 * Logic:
 * Keep all runtime handles in one structure so cleanup and future extension are easier.
 */
typedef struct {
    i2c_master_bus_handle_t i2c_bus_handle;
    esp_lcd_panel_io_handle_t tp_io_handle;
    esp_lcd_touch_handle_t tp_handle;
    uint32_t i2c_scl_speed_hz;
    esp_lcd_touch_config_t tp_cfg;
} cst3530_test_context_t;

/* Function declarations */
static esp_err_t test_i2c_bus_init(cst3530_test_context_t *config);
static esp_err_t test_touch_init(cst3530_test_context_t *config);
static void test_touch_read_task(void *arg);

/* Function:
 * Initialize the I2C master bus
 * Parameters: [config: test application context]
 * Logic:
 * The CST3530 panel IO and the custom device-handle creation path both rely on
 * the same I2C bus handle, so the bus must be initialized first.
 */
static esp_err_t test_i2c_bus_init(cst3530_test_context_t *config)
{
    i2c_master_bus_config_t bus_config = {
        .i2c_port = TEST_I2C_NUM,
        .sda_io_num = TEST_I2C_SDA_GPIO,
        .scl_io_num = TEST_I2C_SCL_GPIO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };

    ESP_RETURN_ON_FALSE(config != NULL, ESP_ERR_INVALID_ARG, TAG, "config is NULL");

    config->i2c_scl_speed_hz = TEST_I2C_CLK_HZ;

    ESP_RETURN_ON_ERROR(
        i2c_new_master_bus(&bus_config, &config->i2c_bus_handle),
        TAG, "i2c_new_master_bus failed"
    );

    ESP_LOGI(TAG, "I2C bus initialized, port=%d, scl=%d, sda=%d, freq=%" PRIu32,
             TEST_I2C_NUM, TEST_I2C_SCL_GPIO, TEST_I2C_SDA_GPIO, config->i2c_scl_speed_hz);

    return ESP_OK;
}

/* Function:
 * Initialize the CST3530 touch controller
 * Parameters: [config: test application context]
 * Logic:
 * 1. Create panel_io from the I2C bus handle
 * 2. Make a local copy of esp_lcd_touch_config_t
 * 3. Pass the I2C bus handle through user_data
 * 4. Call esp_lcd_touch_new_i2c_cst3530() to verify the custom 32-bit register access path
 */
static esp_err_t test_touch_init(cst3530_test_context_t *config)
{
    esp_err_t err = ESP_OK;

    ESP_RETURN_ON_FALSE(config != NULL, ESP_ERR_INVALID_ARG, TAG, "config is NULL");
    ESP_RETURN_ON_FALSE(config->i2c_bus_handle != NULL, ESP_ERR_INVALID_STATE, TAG, "i2c bus is NULL");

    esp_lcd_panel_io_i2c_config_t tp_io_config = ESP_LCD_TOUCH_IO_I2C_CST3530_CONFIG();   // Function: get the default CST3530 IO configuration
    tp_io_config.scl_speed_hz = config->i2c_scl_speed_hz;                                   // Logic: keep panel_io speed consistent with the I2C bus design target

    err = esp_lcd_new_panel_io_i2c(config->i2c_bus_handle, &tp_io_config, &config->tp_io_handle); // Logic: attach the touch controller to the I2C bus handle
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_lcd_new_panel_io_i2c(cst3530) failed (%s)", esp_err_to_name(err));
        return err;
    }

    ESP_LOGI(TAG, "Initialize touch controller CST3530");

    config->tp_cfg = (esp_lcd_touch_config_t) {
        .x_max = TEST_TOUCH_X_MAX,
        .y_max = TEST_TOUCH_Y_MAX,
        .rst_gpio_num = TEST_TOUCH_RST_GPIO,
        .int_gpio_num = TEST_TOUCH_INT_GPIO,
        .levels = {
            .reset = 0,
            .interrupt = 0,
        },
        .flags = {
            .swap_xy = 0,
            .mirror_x = 0,
            .mirror_y = 0,
        },
        .process_coordinates = NULL,
        .interrupt_callback = NULL,
        .user_data = NULL,
    };

    esp_lcd_touch_config_t tp_cfg = config->tp_cfg;                                         // Logic: make a local copy to avoid modifying a read-only/shared object
    tp_cfg.user_data = (void *)config->i2c_bus_handle;                                      // Logic: pass the I2C bus handle so the driver can create a new I2C device internally and bypass the native panel_io read/write path

    err = esp_lcd_touch_new_i2c_cst3530(config->tp_io_handle, &tp_cfg, &config->tp_handle); // Logic: initialize CST3530 and validate the custom 32-bit register access implementation
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "esp_lcd_touch_new_i2c_cst3530 failed (%s)", esp_err_to_name(err));
        return err;
    }

    ESP_LOGI(TAG, "CST3530 touch initialized successfully");
    return ESP_OK;
}

/* Function:
 * Periodically read touch points and print coordinates
 * Parameters: [arg: pointer to cst3530_test_context_t]
 * Logic:
 * This task continuously calls the generic touch framework API so the driver
 * behavior can be verified on real hardware.
 */
static void test_touch_read_task(void *arg)
{
    cst3530_test_context_t *config = (cst3530_test_context_t *)arg;
    uint16_t x[CONFIG_ESP_LCD_TOUCH_MAX_POINTS] = {0};
    uint16_t y[CONFIG_ESP_LCD_TOUCH_MAX_POINTS] = {0};
    uint16_t strength[CONFIG_ESP_LCD_TOUCH_MAX_POINTS] = {0};
    uint8_t point_num = 0;

    if ((config == NULL) || (config->tp_handle == NULL)) {
        ESP_LOGE(TAG, "invalid touch context");
        vTaskDelete(NULL);
        return;
    }

    while (1) {
        esp_err_t err = esp_lcd_touch_read_data(config->tp_handle);
        if (err != ESP_OK) {
            ESP_LOGE(TAG, "esp_lcd_touch_read_data failed (%s)", esp_err_to_name(err));
            vTaskDelay(pdMS_TO_TICKS(200));
            continue;
        }

        bool touched = esp_lcd_touch_get_coordinates(
            config->tp_handle,
            x,
            y,
            strength,
            &point_num,
            CONFIG_ESP_LCD_TOUCH_MAX_POINTS
        );

        if (touched) {
            ESP_LOGI(TAG, "Touch points: %u", point_num);
            for (uint8_t i = 0; i < point_num; i++) {
                ESP_LOGI(TAG, "Point[%u] X=%u Y=%u Strength=%u", i, x[i], y[i], strength[i]);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(30));
    }
}

/* Function:
 * Main entry of the test application
 * Logic:
 * This app is intended for board-level validation of the CST3530 component.
 */
void app_main(void)
{
    static cst3530_test_context_t s_test_ctx = {0};
    esp_err_t err;

    ESP_LOGI(TAG, "CST3530 test app start");

    err = test_i2c_bus_init(&s_test_ctx);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "test_i2c_bus_init failed (%s)", esp_err_to_name(err));
        return;
    }

    err = test_touch_init(&s_test_ctx);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "test_touch_init failed (%s)", esp_err_to_name(err));
        return;
    }

    xTaskCreate(
        test_touch_read_task,
        "cst3530_read_task",
        4096,
        &s_test_ctx,
        5,
        NULL
    );
}
