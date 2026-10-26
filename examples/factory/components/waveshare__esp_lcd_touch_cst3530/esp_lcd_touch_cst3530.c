/*
 * SPDX-FileCopyrightText: 2015-2024 Espressif Systems (Shanghai) CO LTD
 * SPDX-FileCopyrightText: 2025 Waveshare
 *
 * SPDX-License-Identifier: Apache-2.0
 */

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// Warning:
// If data reading becomes abnormal due to incompatibility of the private
// cst3530_panel_io_i2c_t structure (for example after an ESP-IDF internal
// implementation change; the current implementation works with ESP-IDF v5.5.1),
// this file uses an alternate approach.
//
// Pass the I2C bus handle through esp_lcd_touch_config_t::user_data
// (.user_data = (void *)i2c_bus_handle), then create a new CST3530 I2C device
// from that bus handle, and finally use the newly created device handle to
// implement the read/write functions in esp_lcd_touch_cst3530.c.
//
// The main purpose of this approach is to minimize changes to other project files.
//
// Example:
// esp_lcd_touch_config_t tp_cfg = config->tp_cfg;                              // Make a local copy to avoid modifying a read-only object
// tp_cfg.user_data = (void *)config->i2c_bus_handle;                           // Recreate an I2C device from the I2C bus, so the original read/write path is not used
// err = esp_lcd_touch_new_i2c_cst3530(tp_io_handle, &tp_cfg, tp_panel_handle); // Initialize the CST3530 touch controller
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_lcd_touch.h"
#include "esp_lcd_touch_cst3530.h"
#include "driver/i2c_master.h"
#include "esp_lcd_panel_io_interface.h"
#include "esp_lcd_panel_io.h"

static const char *TAG = "CST3530";

// head = 0xD007 offset = 0x00A5 , Reg_addr = 0xD007A500
// head = 0xD007 offset = 0xA5   , Reg_addr = 0xD007A5

// Debug registers
#define CST3530_REG_DEBUG_INFO_MODE                 0xD00000        // [0x00: normal mode] [0xAB: factory mode]
#define CST3530_REG_DEBUG_SOFT_RESET                0xD00003        // [0xAB: soft reset]
#define CST3530_REG_DEBUG_REPORT_MODE               0xD0000C        // [0x00: normal report] [0x01: gesture report]
#define CST3530_REG_DEBUG_SLEEP_MODE_EN             0xD00022        // [0xAB: enter sleep mode]

/* CST3530 registers */
#define ESP_LCD_TOUCH_CST3530_END_READ_REG          0xD00002AB      // End read
#define ESP_LCD_TOUCH_CST3530_DATA_REG              0xD0070000      // All data
#define ESP_LCD_TOUCH_CST3530_MODE_REG              0xD0070200      // Current mode
#define ESP_LCD_TOUCH_CST3530_POINT_CNT_REG         0xD0070300      // bit0~bit3
#define ESP_LCD_TOUCH_CST3530_COORD_REG             0xD0070400      // 4 bytes per point:
                                                                    // [X: byte0 + ((byte3 & 0x0F) << 8)]
                                                                    // [Y: byte1 + ((byte3 & 0xF0) << 4)]
                                                                    // [strength: byte2]
#define ESP_LCD_TOUCH_CST3530_TOUCH_STATE_REG       0xD0070800      // Normal mode:
                                                                    // bit0~bit3 = finger id
                                                                    // bit4~bit7 = finger event , 1: Press, 2: Hold, 0: Lift
                                                                    // Gesture mode:
                                                                    // bit0~bit7 = gesture_id
#define ESP_LCD_TOUCH_CST3530_COORD_NEXT_REG        0xD0070900      // Finger 2 starts here, Finger x layout: [(0xD0000004 + 5*(x-1)) ~ (0xD0000008 + 5*(x-1))]

/* Function:
 * Mirror of the private panel_io_i2c structure
 * Logic:
 * This structure is used to recover the internal I2C device handle from
 * esp_lcd_panel_io_handle_t.
 * Note:
 * This implementation depends on the current ESP-IDF internal layout.
 * Recheck this structure definition after upgrading ESP-IDF.
 */
typedef struct {
    esp_lcd_panel_io_t base;                         // Base class of generic LCD panel IO
    i2c_master_dev_handle_t i2c_handle;             // I2C master driver handle
    uint32_t dev_addr;                              // Device address
    int lcd_cmd_bits;                               // Bit width of LCD command
    int lcd_param_bits;                             // Bit width of LCD parameter
    bool control_phase_enabled;                     // Whether control phase is enabled
    uint32_t control_phase_cmd;                     // Control byte for command transfer
    uint32_t control_phase_data;                    // Control byte for data transfer
    esp_lcd_panel_io_color_trans_done_cb_t on_color_trans_done; // User callback when color transfer is done
    void *user_ctx;                                 // User private context passed to callback
} cst3530_panel_io_i2c_t;

#ifndef CST3530_CONTAINER_OF
#define CST3530_CONTAINER_OF(ptr, type, member) ((type *)((char *)(ptr) - offsetof(type, member)))
#endif

/* Function:
 * CST3530 raw I2C device handle: The current driver is implemented as a single-instance driver, so the I2C device handle is stored in a file-scope global variable.
 */
static i2c_master_dev_handle_t g_cst3530_i2c_dev = NULL;

/*******************************************************************************
 * Function declarations
 *******************************************************************************/
static esp_err_t touch_cst3530_get_i2c_dev_from_io(const esp_lcd_panel_io_handle_t io, i2c_master_dev_handle_t *i2c_dev);
static esp_err_t touch_cst3530_new_i2c_dev_from_bus(i2c_master_bus_handle_t i2c_bus, i2c_master_dev_handle_t *i2c_dev);
static esp_err_t esp_lcd_touch_cst3530_read_data(esp_lcd_touch_handle_t tp);
static bool esp_lcd_touch_cst3530_get_xy(esp_lcd_touch_handle_t tp, uint16_t *x, uint16_t *y, uint16_t *strength, uint8_t *point_num, uint8_t max_point_num);
static esp_err_t esp_lcd_touch_cst3530_del(esp_lcd_touch_handle_t tp);
static esp_err_t touch_cst3530_i2c_read(esp_lcd_touch_handle_t tp, uint32_t reg, uint8_t *data, uint8_t len);
static esp_err_t touch_cst3530_i2c_write(esp_lcd_touch_handle_t tp, uint32_t reg, uint8_t *data, uint8_t len);
static esp_err_t touch_cst3530_reset(esp_lcd_touch_handle_t tp);

/* Function:
 * Extract the I2C device handle from esp_lcd_panel_io_handle_t
 * Parameters: [io: panel IO handle]
 * Parameters: [i2c_dev: output I2C device handle]
 * Logic:
 * This function is only a compatibility fallback. It depends on the current private internal layout of the panel_io I2C object
 * created by esp_lcd_new_panel_io_i2c(), so it may break if ESP-IDF changes that internal structure in a future release.
 * For long-term stability, prefer passing an I2C bus handle through esp_lcd_touch_config_t::user_data and creating a new I2C device handle
 * explicitly inside this driver.
 */
static esp_err_t touch_cst3530_get_i2c_dev_from_io(const esp_lcd_panel_io_handle_t io, i2c_master_dev_handle_t *i2c_dev)
{
    cst3530_panel_io_i2c_t *panel_io_i2c = NULL;
    if ((io == NULL) || (i2c_dev == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }
    panel_io_i2c = CST3530_CONTAINER_OF(io, cst3530_panel_io_i2c_t, base);
    if (panel_io_i2c->i2c_handle == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    *i2c_dev = panel_io_i2c->i2c_handle;
    return ESP_OK;
}

/* Function:
 * Create a CST3530 I2C device handle from an I2C bus handle
 * Parameters: [i2c_bus: I2C bus handle]
 * Parameters: [i2c_dev: output I2C device handle]
 * Logic:
 * When config->user_data carries an I2C bus handle, the driver creates a new CST3530 I2C device handle internally.
 */
static esp_err_t touch_cst3530_new_i2c_dev_from_bus(i2c_master_bus_handle_t i2c_bus, i2c_master_dev_handle_t *i2c_dev)
{
    i2c_device_config_t dev_cfg = {
        .device_address = ESP_LCD_TOUCH_IO_I2C_CST3530_ADDRESS,   // Parameter: fixed CST3530 address = 0x58
        .scl_speed_hz = 400000,                                   // Logic: default to 400 kHz, can be adjusted later if needed
    };
    if ((i2c_bus == NULL) || (i2c_dev == NULL)) {
        return ESP_ERR_INVALID_ARG;
    }
    return i2c_master_bus_add_device(i2c_bus, &dev_cfg, i2c_dev);
}

/*******************************************************************************
 * Public API functions
 *******************************************************************************/

esp_err_t esp_lcd_touch_new_i2c_cst3530(const esp_lcd_panel_io_handle_t io, const esp_lcd_touch_config_t *config, esp_lcd_touch_handle_t *out_touch)
{
    esp_err_t ret = ESP_OK;
    esp_lcd_touch_handle_t esp_lcd_touch_cst3530 = NULL;

    assert(io != NULL);
    assert(config != NULL);
    assert(out_touch != NULL);

    /* Function:
     * Determine how to obtain the CST3530 I2C device handle
     * Parameters: [config->user_data: optional I2C bus handle provided by the caller]
     * Parameters: [io: panel IO handle that may internally contain an I2C device handle]
     * Logic:
     * Prefer config->user_data when available, because it allows creating a new CST3530 I2C device without depending on the internal panel_io structure.
     * Otherwise, fall back to extracting the device handle from io.
     */
    if (config->user_data != NULL) {
        i2c_master_bus_handle_t i2c_bus = (i2c_master_bus_handle_t)config->user_data;    // Parameter: I2C bus handle passed through config->user_data
        ret = touch_cst3530_new_i2c_dev_from_bus(i2c_bus, &g_cst3530_i2c_dev);           // Logic: create a dedicated CST3530 I2C device handle from the bus handle
        ESP_GOTO_ON_ERROR(ret, err, TAG, "Create CST3530 I2C device from bus failed");
        ESP_LOGI("CST3530 transfer parameters", "esp_lcd_touch_config_t *config");        // Logic: log that the config-based transfer path is used
    } else {
        ret = touch_cst3530_get_i2c_dev_from_io(io, &g_cst3530_i2c_dev);                  // Logic: reuse the I2C device handle hidden inside panel_io
        ESP_GOTO_ON_ERROR(ret, err, TAG, "Get CST3530 I2C device from panel_io failed");
        ESP_LOGI("CST3530 transfer parameters", "esp_lcd_panel_io_handle_t io");          // Logic: log that the panel_io-based transfer path is used
    }
    esp_lcd_touch_cst3530 = heap_caps_calloc(1, sizeof(esp_lcd_touch_t), MALLOC_CAP_DEFAULT);
    ESP_GOTO_ON_FALSE(esp_lcd_touch_cst3530, ESP_ERR_NO_MEM, err, TAG, "No memory for CST3530 controller");
    /* Communication interface */
    esp_lcd_touch_cst3530->io = io;   // Logic: keep the original io handle for compatibility with upper-layer semantics, although low-level access no longer uses it
    /* Only supported callbacks are set */
    esp_lcd_touch_cst3530->read_data = esp_lcd_touch_cst3530_read_data;
    esp_lcd_touch_cst3530->get_xy = esp_lcd_touch_cst3530_get_xy;
    esp_lcd_touch_cst3530->del = esp_lcd_touch_cst3530_del;

    esp_lcd_touch_cst3530->data.lock.owner = portMUX_FREE_VAL;    /* Mutex */
    memcpy(&esp_lcd_touch_cst3530->config, config, sizeof(esp_lcd_touch_config_t));    /* Save config */
    ESP_LOGI(TAG, "Touch driver config: rst_gpio_num=%d, GPIO_NUM_NC=%d",
             (int)esp_lcd_touch_cst3530->config.rst_gpio_num, (int)GPIO_NUM_NC);
    /* Prepare pin for touch interrupt */
    if (esp_lcd_touch_cst3530->config.int_gpio_num != GPIO_NUM_NC) {
        const gpio_config_t int_gpio_config = {
            .mode = GPIO_MODE_INPUT,
            .intr_type = GPIO_INTR_NEGEDGE,
            .pin_bit_mask = BIT64(esp_lcd_touch_cst3530->config.int_gpio_num)
        };
        ret = gpio_config(&int_gpio_config);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "GPIO config failed");
    }
    /* Prepare pin for touch reset */
    if (esp_lcd_touch_cst3530->config.rst_gpio_num != GPIO_NUM_NC) {
        const gpio_config_t rst_gpio_config = {
            .mode = GPIO_MODE_OUTPUT,
            .pull_up_en = GPIO_PULLUP_ENABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_DISABLE,
            .pin_bit_mask = BIT64(esp_lcd_touch_cst3530->config.rst_gpio_num)
        };
        ret = gpio_config(&rst_gpio_config);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "RST GPIO config failed");
    }
    if (esp_lcd_touch_cst3530->config.rst_gpio_num != GPIO_NUM_NC) {
        ret = touch_cst3530_reset(esp_lcd_touch_cst3530);
        ESP_GOTO_ON_ERROR(ret, err, TAG, "CST3530 reset failed");
    }
    *out_touch = esp_lcd_touch_cst3530;
    return ESP_OK;
err:
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error (0x%x)! Touch controller CST3530 initialization failed!", ret);
        if (esp_lcd_touch_cst3530) {
            esp_lcd_touch_cst3530_del(esp_lcd_touch_cst3530);
        }
    }
    *out_touch = NULL;
    return ret;
}

/*******************************************************************************
 * Private functions
 *******************************************************************************/

static esp_err_t esp_lcd_touch_cst3530_read_data(esp_lcd_touch_handle_t tp)
{
    esp_err_t err;
    uint8_t buf[51];
    uint8_t touch_cnt = 0;
    size_t i = 0;
    assert(tp != NULL);
    taskENTER_CRITICAL(&tp->data.lock);
    tp->data.points = 0;
    taskEXIT_CRITICAL(&tp->data.lock);
    err = touch_cst3530_i2c_read(tp, ESP_LCD_TOUCH_CST3530_DATA_REG, buf, 9);
    ESP_RETURN_ON_ERROR(err, TAG, "I2C read error!");
    /* Any touch data available? */
    if ((buf[3] & 0x0F) == 0x00 || (buf[8] & 0xF0) == 0x00 ) {
        err = touch_cst3530_i2c_write(tp, ESP_LCD_TOUCH_CST3530_END_READ_REG, NULL, 0);
        ESP_RETURN_ON_ERROR(err, TAG, "I2C write error!");
        return ESP_OK;
    } else {
        /* Number of touched points */
        touch_cnt = buf[3] & 0x0F;
        if (touch_cnt > CONFIG_ESP_LCD_TOUCH_MAX_POINTS || touch_cnt == 0) {
            err = touch_cst3530_i2c_write(tp, ESP_LCD_TOUCH_CST3530_END_READ_REG, NULL, 0);
            ESP_RETURN_ON_ERROR(err, TAG, "I2C write error!");
            return ESP_OK;
        }
        /* Read all point data */
        if(touch_cnt > 1){
            err = touch_cst3530_i2c_read(tp, ESP_LCD_TOUCH_CST3530_COORD_NEXT_REG, &buf[9], (touch_cnt - 1) * 5);
            ESP_RETURN_ON_ERROR(err, TAG, "I2C read error!");
        }
        err = touch_cst3530_i2c_write(tp, ESP_LCD_TOUCH_CST3530_END_READ_REG, NULL, 0);
        ESP_RETURN_ON_ERROR(err, TAG, "I2C read error!");
        taskENTER_CRITICAL(&tp->data.lock);
        /* Save the number of touched points */
        tp->data.points = (uint8_t)touch_cnt;
        /* Parse all coordinate data */
        for (i = 0; i < touch_cnt; i++) {
            tp->data.coords[i].x = (uint16_t)(((buf[(i * 5) + 7] & 0x0F) << 8) + buf[(i * 5) + 4]);
            tp->data.coords[i].y = (uint16_t)(((buf[(i * 5) + 7] & 0xF0) << 4) + buf[(i * 5) + 5]);
            tp->data.coords[i].strength = (uint16_t)buf[(i * 5) + 6];
            tp->data.coords[i].track_id = (uint8_t)(buf[(i * 5) + 8] & 0x0F);
        }
        taskEXIT_CRITICAL(&tp->data.lock);
    }
    return ESP_OK;
}

static bool esp_lcd_touch_cst3530_get_xy(esp_lcd_touch_handle_t tp, uint16_t *x, uint16_t *y, uint16_t *strength, uint8_t *point_num, uint8_t max_point_num)
{
    assert(tp != NULL);
    assert(x != NULL);
    assert(y != NULL);
    assert(point_num != NULL);
    assert(max_point_num > 0);

    taskENTER_CRITICAL(&tp->data.lock);
    /* Limit the reported point count to the caller buffer size */
    if (tp->data.points > max_point_num) {
        tp->data.points = max_point_num;
    }
    for (size_t i = 0; i < tp->data.points; i++) {
        x[i] = tp->data.coords[i].x;
        y[i] = tp->data.coords[i].y;
        if (strength) {
            strength[i] = tp->data.coords[i].strength;
        }
    }
    *point_num = tp->data.points;
    /* Invalidate cached points after reading */
    tp->data.points = 0;
    taskEXIT_CRITICAL(&tp->data.lock);
    return (*point_num > 0);
}

static esp_err_t esp_lcd_touch_cst3530_del(esp_lcd_touch_handle_t tp)
{
    assert(tp != NULL);

    /* Reset INT GPIO configuration */
    if (tp->config.int_gpio_num != GPIO_NUM_NC) {
        gpio_reset_pin(tp->config.int_gpio_num);
    }
    /* Reset RST GPIO configuration */
    if (tp->config.rst_gpio_num != GPIO_NUM_NC) {
        gpio_reset_pin(tp->config.rst_gpio_num);
    }
    g_cst3530_i2c_dev = NULL;   // Logic: clear the internally stored I2C device handle
    free(tp);
    return ESP_OK;
}

static esp_err_t touch_cst3530_reset(esp_lcd_touch_handle_t tp)
{
    assert(tp != NULL);

    ESP_LOGI(TAG, "Touch reset: rst_gpio_num=%d", (int)tp->config.rst_gpio_num);
    // The board resets CST3530 through the CH32V003 IO expander.
    if (tp->config.rst_gpio_num == GPIO_NUM_NC) {
        return ESP_OK;
    }

    ESP_RETURN_ON_ERROR(gpio_set_level(tp->config.rst_gpio_num, tp->config.levels.reset), TAG, "GPIO set level error!");
    vTaskDelay(pdMS_TO_TICKS(100));
    ESP_RETURN_ON_ERROR(gpio_set_level(tp->config.rst_gpio_num, !tp->config.levels.reset), TAG, "GPIO set level error!");
    vTaskDelay(pdMS_TO_TICKS(500));
    return ESP_OK;
}

/* Function:
 * Read data from a CST3530 register
 * Parameters: [tp: touch handle]
 * Parameters: [reg: 32-bit register address]
 * Parameters: [data: receive buffer]
 * Parameters: [len: number of bytes to read]
 * Logic:
 * The native/original panel I/O access function does not support the 32-bit
 * register addressing required by CST3530, so a custom I2C read routine is
 * implemented here.
 * The routine first transmits the 4-byte register address, then receives the
 * requested data bytes. It does not use the original panel_io cmd/param packing
 * mechanism anymore.
 */
static esp_err_t touch_cst3530_i2c_read(esp_lcd_touch_handle_t tp, uint32_t reg, uint8_t *data, uint8_t len)
{
    uint8_t reg_buf[4];

    assert(tp != NULL);
    assert(data != NULL);

    if (g_cst3530_i2c_dev == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    reg_buf[0] = (uint8_t)((reg >> 24) & 0xFF);    // Logic: register address byte [31:24]
    reg_buf[1] = (uint8_t)((reg >> 16) & 0xFF);    // Logic: register address byte [23:16]
    reg_buf[2] = (uint8_t)((reg >> 8) & 0xFF);     // Logic: register address byte [15:8]
    reg_buf[3] = (uint8_t)(reg & 0xFF);            // Logic: register address byte [7:0]
    ESP_RETURN_ON_ERROR(i2c_master_transmit(g_cst3530_i2c_dev, reg_buf, sizeof(reg_buf), -1), TAG, "I2C write reg failed");
    ESP_RETURN_ON_ERROR(i2c_master_receive(g_cst3530_i2c_dev, data, len, -1), TAG, "I2C read data failed");
    return ESP_OK;
}

/* Function:
 * Write data to a CST3530 register
 * Parameters: [tp: touch handle]
 * Parameters: [reg: 32-bit register address]
 * Parameters: [data: optional payload data to append, can be NULL]
 * Parameters: [len: payload length to append, can be 0]
 * Logic:
 * The native/original panel I/O access function does not support the 32-bit
 * register addressing required by CST3530, so a custom I2C write routine is
 * implemented here.
 * The routine first transmits the 4-byte register address, then appends payload
 * data if needed.
 */
static esp_err_t touch_cst3530_i2c_write(esp_lcd_touch_handle_t tp, uint32_t reg, uint8_t *data, uint8_t len)
{
    uint8_t tx_buf[20] = {0};

    assert(tp != NULL);

    if (g_cst3530_i2c_dev == NULL) {
        return ESP_ERR_INVALID_STATE;
    }
    if (len > 16) {
        return ESP_ERR_INVALID_SIZE;
    }
    tx_buf[0] = (uint8_t)((reg >> 24) & 0xFF);     // Logic: register address byte [31:24]
    tx_buf[1] = (uint8_t)((reg >> 16) & 0xFF);     // Logic: register address byte [23:16]
    tx_buf[2] = (uint8_t)((reg >> 8) & 0xFF);      // Logic: register address byte [15:8]
    tx_buf[3] = (uint8_t)(reg & 0xFF);             // Logic: register address byte [7:0]
    if ((data != NULL) && (len > 0)) {
        memcpy(&tx_buf[4], data, len);             // Logic: append payload data after the 4-byte register address
    }
    ESP_RETURN_ON_ERROR(i2c_master_transmit(g_cst3530_i2c_dev, tx_buf, 4 + len, -1), TAG, "I2C write data failed" );
    return ESP_OK;
}
