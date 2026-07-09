#include "ch32v003.h"

#define TEST_LOOP_CNT       10
#define TEST_LOOP_DELAY_MS  500

static const char *TAG = "custom_io";

esp_io_expander_handle_t io_expander = NULL;
i2c_master_bus_handle_t i2c_handle = NULL;

/**
 * @brief  Initialize I2C master bus
 */
void i2c_bus_init(void)
{
    const i2c_master_bus_config_t bus_config = {
        .i2c_port = I2C_MASTER_NUM,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .clk_source = I2C_CLK_SRC_DEFAULT,
    };

    esp_err_t ret = i2c_new_master_bus(&bus_config, &i2c_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "I2C bus initialization failed! Error code: 0x%x", ret);
        return;
    }
    ESP_LOGI(TAG, "I2C bus initialization completed");
}

/**
 * @brief  Initialize CH32V003 IO expander
 */
void i2c_dev_custom_io_init(void)
{
    esp_err_t ret = custom_io_expander_new_i2c_ch32v003(i2c_handle, I2C_ADDRESS, &io_expander);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "IO expander initialization failed! Error code: 0x%x", ret);
        return;
    }
    ESP_LOGI(TAG, "IO expander initialization completed");
}

/**
 * @brief  CH32V003 initialization entry function
 */
void ch32v003_init(void)
{
    i2c_bus_init();
    i2c_dev_custom_io_init();
}

void ch32_test()
{
    esp_err_t ret;
    uint32_t input_level_mask = 0;

    ret = esp_io_expander_set_dir(io_expander, 
                                  IO_EXP_PIN_0 | IO_EXP_PIN_1 | IO_EXP_PIN_2 | IO_EXP_PIN_3 |
                                  IO_EXP_PIN_4 | IO_EXP_PIN_5 | IO_EXP_PIN_6 | IO_EXP_PIN_7, 
                                  IO_EXPANDER_OUTPUT);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Successfully configured IO_EXP_PIN_0~7 as output mode");
    } else {
        ESP_LOGE(TAG, "Failed to configure IO_EXP_PIN_0~7 as output mode, ret=0x%x", ret);
    }

    ret = esp_io_expander_print_state(io_expander);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "IO expander state printed successfully");
    } else {
        ESP_LOGE(TAG, "Failed to print IO expander state, ret=0x%x", ret);
    }

    for (int i = 0; i < TEST_LOOP_CNT; i++) {
        // Set level to 0
        ESP_LOGI(TAG, "Loop %d: Set IO_EXP_PIN_0~7 level to 0", i+1);
        ret = esp_io_expander_set_level(io_expander, 
                                        IO_EXP_PIN_0 | IO_EXP_PIN_1 | IO_EXP_PIN_2 | IO_EXP_PIN_3 |
                                        IO_EXP_PIN_4 | IO_EXP_PIN_5 | IO_EXP_PIN_6 | IO_EXP_PIN_7, 
                                        0);
        if (ret == ESP_OK) {
            ESP_LOGI(TAG, "IO_EXP_PIN_0~7 level set to 0 successfully");
        } else {
            ESP_LOGE(TAG, "Failed to set IO_EXP_PIN_0~7 level to 0, ret=0x%x", ret);
        }
        vTaskDelay(pdMS_TO_TICKS(TEST_LOOP_DELAY_MS / 2));

        // Set level to 1
        ESP_LOGI(TAG, "Loop %d: Set IO_EXP_PIN_0~7 level to 1", i+1);
        ret = esp_io_expander_set_level(io_expander, 
                                        IO_EXP_PIN_0 | IO_EXP_PIN_1 | IO_EXP_PIN_2 | IO_EXP_PIN_3 |
                                        IO_EXP_PIN_4 | IO_EXP_PIN_5 | IO_EXP_PIN_6 | IO_EXP_PIN_7, 
                                        1);
        if (ret == ESP_OK) {
            ESP_LOGI(TAG, "IO_EXP_PIN_0~7 level set to 1 successfully");
        } else {
            ESP_LOGE(TAG, "Failed to set IO_EXP_PIN_0~7 level to 1, ret=0x%x", ret);
        }
        vTaskDelay(pdMS_TO_TICKS(TEST_LOOP_DELAY_MS / 2));
    }

    ret = esp_io_expander_set_dir(io_expander, 
                                  IO_EXP_PIN_0 | IO_EXP_PIN_1 | IO_EXP_PIN_2 | IO_EXP_PIN_3 |
                                  IO_EXP_PIN_4 | IO_EXP_PIN_5 | IO_EXP_PIN_6 | IO_EXP_PIN_7, 
                                  IO_EXPANDER_INPUT);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "Successfully configured IO_EXP_PIN_0~7 as input mode");
    } else {
        ESP_LOGE(TAG, "Failed to configure IO_EXP_PIN_0~7 as input mode, ret=0x%x", ret);
    }

    ret = esp_io_expander_print_state(io_expander);
    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "IO expander state (input mode) printed successfully");
    } else {
        ESP_LOGE(TAG, "Failed to print IO expander state (input mode), ret=0x%x", ret);
    }

    for (int i = 0; i < TEST_LOOP_CNT; i++) {
        ESP_LOGI(TAG, "Loop %d: Read IO_EXP_PIN_0~7 input levels", i+1);
        ret = esp_io_expander_get_level(io_expander, 
                                        IO_EXP_PIN_0 | IO_EXP_PIN_1 | IO_EXP_PIN_2 | IO_EXP_PIN_3 |
                                        IO_EXP_PIN_4 | IO_EXP_PIN_5 | IO_EXP_PIN_6 | IO_EXP_PIN_7, 
                                        &input_level_mask);
        if (ret == ESP_OK) {
            ESP_LOGI(TAG, "IO_EXP_PIN_0~7 input levels read successfully, mask value: 0x%02x", input_level_mask);
        } else {
            ESP_LOGE(TAG, "Failed to read IO_EXP_PIN_0~7 input levels, ret=0x%x", ret);
        }
        vTaskDelay(pdMS_TO_TICKS(TEST_LOOP_DELAY_MS));
    }

    ESP_LOGI(TAG, "Test completed");
}