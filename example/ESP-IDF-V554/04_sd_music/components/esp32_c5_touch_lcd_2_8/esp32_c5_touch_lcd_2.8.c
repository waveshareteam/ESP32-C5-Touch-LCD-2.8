#include <stdio.h>
#include "dirent.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_io_additions.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_vfs_fat.h"
#include "esp_spiffs.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_lcd_panel_st7789.h"
#include "esp_lcd_touch_cst3530.h"

#include "esp_codec_dev_defaults.h"
#include "bsp/esp32_c5_touch_lcd_2.8.h"
#include "bsp_err_check.h"
#include "bsp/display.h"
#include "bsp/touch.h"
#include "iot_button.h"
#include "sdmmc_cmd.h"

static const char *TAG = "ESP32-C5-Touch-LCD-2.8";

static i2c_master_bus_handle_t i2c_handle = NULL;
static bool i2c_initialized = false;
static bool spi_sd_initialized = false;
static esp_io_expander_handle_t io_expander = NULL;
static lv_indev_t *disp_indev = NULL;
static lv_display_t *disp_drv = NULL;
sdmmc_card_t *bsp_sdcard = NULL;
static esp_lcd_touch_handle_t tp = NULL;
static esp_lcd_panel_handle_t panel_handle = NULL;
static esp_lcd_panel_io_handle_t io_handle = NULL;
static i2s_chan_handle_t i2s_tx_chan = NULL;
static i2s_chan_handle_t i2s_rx_chan = NULL;
static const audio_codec_data_if_t *i2s_data_if = NULL;
static uint8_t brightness;

qmi8658_dev_t qmi_dev;
pcf85063a_dev_t pcf_dev;
i2c_master_dev_handle_t shtc3_handle = NULL;

#define BSP_I2S_GPIO_CFG       \
    {                          \
        .mclk = BSP_I2S_MCLK,  \
        .bclk = BSP_I2S_SCLK,  \
        .ws = BSP_I2S_LCLK,    \
        .dout = BSP_I2S_DOUT,  \
        .din = BSP_I2S_DSIN,   \
        .invert_flags = {      \
            .mclk_inv = false, \
            .bclk_inv = false, \
            .ws_inv = false,   \
        },                     \
    }

/* This configuration is used by default in bsp_audio_init() */
#define BSP_I2S_DUPLEX_MONO_CFG(_sample_rate)                                                         \
    {                                                                                                 \
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(_sample_rate),                                          \
        .slot_cfg = I2S_STD_PHILIP_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_MONO), \
        .gpio_cfg = BSP_I2S_GPIO_CFG,                                                                 \
    }

/**************************************************************************************************
 * I2C init
 **************************************************************************************************/
esp_err_t bsp_i2c_init(void)
{
    if (i2c_initialized) {
        return ESP_OK;
    }
    const i2c_master_bus_config_t i2c_config = {
        .i2c_port = BSP_I2C_NUM,
        .sda_io_num = BSP_I2C_SDA,
        .scl_io_num = BSP_I2C_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    BSP_ERROR_CHECK_RETURN_ERR(i2c_new_master_bus(&i2c_config, &i2c_handle));
    i2c_initialized = true;
    return ESP_OK;
}

esp_err_t bsp_i2c_deinit(void)
{
    BSP_ERROR_CHECK_RETURN_ERR(i2c_del_master_bus(i2c_handle));
    i2c_initialized = false;
    return ESP_OK;
}

i2c_master_bus_handle_t bsp_i2c_get_handle(void)
{
    bsp_i2c_init();
    return i2c_handle;
}

/**************************************************************************************************
 * SPI + SD卡
 **************************************************************************************************/
sdmmc_card_t *bsp_sdcard_get_handle(void)
{
    return bsp_sdcard;
}

static esp_err_t bsp_spi_init(void)
{
    if (spi_sd_initialized) {
        return ESP_OK;
    }
    ESP_LOGD(TAG, "Initialize SPI bus");
    const spi_bus_config_t buscfg = {
        .sclk_io_num = BSP_LCD_PCLK,
        .mosi_io_num = BSP_LCD_MOSI,
        .miso_io_num = BSP_SD_MISO,
        .quadwp_io_num = GPIO_NUM_NC,
        .quadhd_io_num = GPIO_NUM_NC,
    };
    ESP_RETURN_ON_ERROR(spi_bus_initialize(BSP_LCD_SPI_NUM, &buscfg, SPI_DMA_CH_AUTO), TAG, "SPI init failed");
    spi_sd_initialized = true;
    return ESP_OK;
}

static esp_err_t bsp_spi_deinit(void)
{
    BSP_ERROR_CHECK_RETURN_ERR(spi_bus_free(BSP_LCD_SPI_NUM));
    spi_sd_initialized = false;
    return ESP_OK;
}

void bsp_sdcard_get_sdspi_host(const int slot, sdmmc_host_t *config)
{
    assert(config);
    sdmmc_host_t host_config = SDSPI_HOST_DEFAULT();
    host_config.slot = slot;
    memcpy(config, &host_config, sizeof(sdmmc_host_t));
}

void bsp_sdcard_sdspi_get_slot(const spi_host_device_t spi_host, sdspi_device_config_t *config)
{
    assert(config);
    memset(config, 0, sizeof(sdspi_device_config_t));
    config->gpio_cs   = BSP_SD_SDCS;
    config->gpio_cd   = SDSPI_SLOT_NO_CD;
    config->gpio_wp   = SDSPI_SLOT_NO_WP;
    config->gpio_int  = GPIO_NUM_NC;
    config->host_id = spi_host;
#if ESP_IDF_VERSION >= ESP_IDF_VERSION_VAL(5, 2, 0)
    config->gpio_wp_polarity = SDSPI_IO_ACTIVE_LOW;
#endif
}

esp_err_t bsp_sdcard_sdspi_mount(bsp_sdcard_cfg_t *cfg)
{
    sdmmc_host_t sdhost = {0};
    sdspi_device_config_t sdslot = {0};
    const esp_vfs_fat_sdmmc_mount_config_t mount_config = {
#ifdef CONFIG_BSP_SD_FORMAT_ON_MOUNT_FAIL
        .format_if_mount_failed = true,
#else
        .format_if_mount_failed = false,
#endif
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    assert(cfg);

    if(!spi_sd_initialized) {
        bsp_spi_init();
    }
    spi_sd_initialized = true;

    if (!cfg->mount) cfg->mount = &mount_config;
    if (!cfg->host) {
        bsp_sdcard_get_sdspi_host(SDSPI_DEFAULT_HOST, &sdhost);
        cfg->host = &sdhost;
    }
    if (!cfg->slot.sdspi) {
        bsp_sdcard_sdspi_get_slot(BSP_LCD_SPI_NUM, &sdslot);
        cfg->slot.sdspi = &sdslot;
    }

#if !CONFIG_FATFS_LONG_FILENAMES
    ESP_LOGW(TAG, "Warning: Long filenames on SD card are disabled in menuconfig!");
#endif

    ESP_RETURN_ON_ERROR(esp_vfs_fat_sdspi_mount(BSP_SD_MOUNT_POINT, cfg->host, cfg->slot.sdspi, cfg->mount, &bsp_sdcard),
                        TAG, "SD card SPI mount failed");

    sdmmc_card_print_info(stdout, bsp_sdcard);
    return ESP_OK;
}

esp_err_t bsp_sdcard_mount(void)
{
    bsp_sdcard_cfg_t cfg = {0};
    return bsp_sdcard_sdspi_mount(&cfg);
}

esp_err_t bsp_sdcard_unmount(void)
{
    esp_err_t ret = ESP_OK;
    ret |= esp_vfs_fat_sdcard_unmount(BSP_SD_MOUNT_POINT, bsp_sdcard);
    bsp_sdcard = NULL;
    return ret;
}

/**************************************************************************************************
 * Get file
 **************************************************************************************************/
esp_err_t get_file_list_by_ext(const char *dir_path, const char *extension, generic_file_list_t *out)
{
    if (!dir_path || !extension || !out) return ESP_ERR_INVALID_ARG;
    out->list  = NULL;
    out->count = 0;

    DIR *dir = opendir(dir_path);
    if (!dir) return ESP_FAIL;

    struct dirent *entry;
    int count = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            char *ext = strrchr(entry->d_name, '.');
            if (ext && strcasecmp(ext, extension) == 0) count++;
        }
    }
    if (count == 0) {
        closedir(dir);
        return ESP_ERR_NOT_FOUND;
    }

    char **list = (char **)malloc(sizeof(char *) * count);
    if (!list) {
        closedir(dir);
        return ESP_ERR_NO_MEM;
    }

    rewinddir(dir);
    int idx = 0;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_REG) {
            char *ext = strrchr(entry->d_name, '.');
            if (ext && strcasecmp(ext, extension) == 0) {
                size_t len = strlen(dir_path) + strlen(entry->d_name) + 2;
                char *full_path = (char *)malloc(len);
                snprintf(full_path, len, "%s/%s", dir_path, entry->d_name);
                list[idx++] = full_path;
            }
        }
    }
    closedir(dir);
    out->list  = list;
    out->count = idx;
    return ESP_OK;
}

/**************************************************************************************************
 * SPIFFS
 **************************************************************************************************/
esp_err_t bsp_spiffs_mount(void)
{
    esp_vfs_spiffs_conf_t conf = {
        .base_path = CONFIG_BSP_SPIFFS_MOUNT_POINT,
        .partition_label = CONFIG_BSP_SPIFFS_PARTITION_LABEL,
        .max_files = CONFIG_BSP_SPIFFS_MAX_FILES,
#ifdef CONFIG_BSP_SPIFFS_FORMAT_ON_MOUNT_FAIL
        .format_if_mount_failed = true,
#else
        .format_if_mount_failed = false,
#endif
    };
    esp_err_t ret_val = esp_vfs_spiffs_register(&conf);
    BSP_ERROR_CHECK_RETURN_ERR(ret_val);

    size_t total = 0, used = 0;
    ret_val = esp_spiffs_info(conf.partition_label, &total, &used);
    if (ret_val != ESP_OK) {
        ESP_LOGE(TAG, "Failed to get SPIFFS partition information");
    } else {
        ESP_LOGI(TAG, "Partition size: total: %d, used: %d", total, used);
    }
    return ret_val;
}

esp_err_t bsp_spiffs_unmount(void)
{
    return esp_vfs_spiffs_unregister(CONFIG_BSP_SPIFFS_PARTITION_LABEL);
}

/**************************************************************************************************
 * CH32V003 init
 **************************************************************************************************/
esp_err_t bsp_io_expander_init(void)
{
    if (io_expander != NULL) return ESP_OK;

    BSP_ERROR_CHECK_RETURN_ERR(bsp_i2c_init());
    BSP_ERROR_CHECK_RETURN_ERR(custom_io_expander_new_i2c_ch32v003(
        i2c_handle,
        BSP_IO_EXPANDER_I2C_ADDRESS,
        &io_expander
    ));

    esp_io_expander_set_dir(io_expander,
        IO_LCD_RST | IO_LCD_TOUCH_RST | IO_POWER_AMP_IO,
        IO_EXPANDER_OUTPUT
    );
    // 默认电平
    esp_io_expander_set_level(io_expander, IO_LCD_RST | IO_LCD_TOUCH_RST, 1);
    esp_io_expander_set_level(io_expander, IO_POWER_AMP_IO, 1);

    ESP_LOGI(TAG, "CH32V003 IO expander init success");
    return ESP_OK;
}

esp_io_expander_handle_t bsp_get_io_expander_handle(void)
{
    return io_expander;
}

/**************************************************************************************************
 * I2S init
 **************************************************************************************************/

 esp_err_t bsp_audio_init(const i2s_std_config_t *i2s_config)
{
    esp_err_t ret = ESP_FAIL;
    if (i2s_tx_chan && i2s_rx_chan) {
        return ESP_OK;
    }

    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(BSP_I2S_NUM, I2S_ROLE_MASTER);
    chan_cfg.auto_clear = true;
    BSP_ERROR_CHECK_RETURN_ERR(i2s_new_channel(&chan_cfg, &i2s_tx_chan, &i2s_rx_chan));

    const i2s_std_config_t std_cfg_default = BSP_I2S_DUPLEX_MONO_CFG(22050);
    const i2s_std_config_t *p_i2s_cfg = &std_cfg_default;
    if (i2s_config != NULL) {
        p_i2s_cfg = i2s_config;
    }

    if (i2s_tx_chan != NULL) {
        ESP_GOTO_ON_ERROR(i2s_channel_init_std_mode(i2s_tx_chan, p_i2s_cfg), err, TAG, "I2S channel initialization failed");
        ESP_GOTO_ON_ERROR(i2s_channel_enable(i2s_tx_chan), err, TAG, "I2S enabling failed");
    }
    if (i2s_rx_chan != NULL) {
        ESP_GOTO_ON_ERROR(i2s_channel_init_std_mode(i2s_rx_chan, p_i2s_cfg), err, TAG, "I2S channel initialization failed");
        ESP_GOTO_ON_ERROR(i2s_channel_enable(i2s_rx_chan), err, TAG, "I2S enabling failed");
    }

    audio_codec_i2s_cfg_t i2s_cfg = {
        .port = BSP_I2S_NUM,
        .rx_handle = i2s_rx_chan,
        .tx_handle = i2s_tx_chan,
    };
    i2s_data_if = audio_codec_new_i2s_data(&i2s_cfg);
    BSP_NULL_CHECK_GOTO(i2s_data_if, err);

    return ESP_OK;

err:
    if (i2s_tx_chan) i2s_del_channel(i2s_tx_chan);
    if (i2s_rx_chan) i2s_del_channel(i2s_rx_chan);
    return ret;
}

const audio_codec_data_if_t *bsp_audio_get_codec_itf(void)
{
    return i2s_data_if;
}



esp_codec_dev_handle_t bsp_audio_codec_speaker_init(void)
{
    const audio_codec_data_if_t *i2s_data_if = bsp_audio_get_codec_itf();
    if (i2s_data_if == NULL) {
        BSP_ERROR_CHECK_RETURN_NULL(bsp_i2c_init());
        BSP_ERROR_CHECK_RETURN_NULL(bsp_audio_init(NULL));
        i2s_data_if = bsp_audio_get_codec_itf();
    }
    assert(i2s_data_if);

    const audio_codec_gpio_if_t *gpio_if = audio_codec_new_gpio();
    audio_codec_i2c_cfg_t i2c_cfg = {
        .port = BSP_I2C_NUM,
        .addr = ES8311_CODEC_DEFAULT_ADDR,
        .bus_handle = i2c_handle,
    };
    const audio_codec_ctrl_if_t *i2c_ctrl_if = audio_codec_new_i2c_ctrl(&i2c_cfg);
    BSP_NULL_CHECK(i2c_ctrl_if, NULL);

    esp_codec_dev_hw_gain_t gain = {
        .pa_voltage = 5.0,
        .codec_dac_voltage = 3.3,
    };

    es8311_codec_cfg_t es8311_cfg = {
        .ctrl_if = i2c_ctrl_if,
        .gpio_if = gpio_if,
        .codec_mode = ESP_CODEC_DEV_WORK_MODE_DAC,
        .pa_pin = BSP_POWER_AMP_IO,
        .pa_reverted = false,
        .master_mode = false,
        .use_mclk = false,
        .digital_mic = false,
        .invert_mclk = false,
        .invert_sclk = false,
        .hw_gain = gain,
    };
    const audio_codec_if_t *es8311_dev = es8311_codec_new(&es8311_cfg);
    BSP_NULL_CHECK(es8311_dev, NULL);

    esp_codec_dev_cfg_t codec_dev_cfg = {
        .dev_type = ESP_CODEC_DEV_TYPE_OUT,
        .codec_if = es8311_dev,
        .data_if = i2s_data_if,
    };
    return esp_codec_dev_new(&codec_dev_cfg);
} 


/**************************************************************************************************
 * Sensor init
 **************************************************************************************************/

 pcf85063a_datetime_t Set_Time = {
    .year = 2026, .month = 04, .day = 22, .dotw = 1,
    .hour = 23, .min = 59, .sec = 0
};

esp_err_t qmi_init()
{
    i2c_master_bus_handle_t bus_handle = bsp_i2c_get_handle();
    if (!bus_handle) return ESP_FAIL;
    esp_err_t ret = qmi8658_init(&qmi_dev, bus_handle, QMI8658_ADDRESS_HIGH);
    if (ret != ESP_OK) return ret;

    qmi8658_set_accel_range(&qmi_dev, QMI8658_ACCEL_RANGE_8G);
    qmi8658_set_accel_odr(&qmi_dev, QMI8658_ACCEL_ODR_500HZ);
    qmi8658_set_accel_unit_mps2(&qmi_dev, true);
    qmi8658_write_register(&qmi_dev, QMI8658_CTRL5, 0x03);
    ESP_LOGI(TAG, "qmi8658 init success");
    return ESP_OK;
}

esp_err_t pcf_init()
{
    i2c_master_bus_handle_t bus_handle = bsp_i2c_get_handle();
    esp_err_t ret = pcf85063a_init(&pcf_dev, bus_handle, PCF85063A_ADDRESS);
    if (ret != ESP_OK) return ret;
    pcf85063a_set_time_date(&pcf_dev, Set_Time);
    ESP_LOGI(TAG, "pcf85063a init success");
    return ESP_OK;
}

esp_err_t shtc3_init(void)
{
    i2c_master_bus_handle_t bus_handle = bsp_i2c_get_handle();
    if (!bus_handle) return ESP_FAIL;

    shtc3_handle = shtc3_device_create(bus_handle, SHTC3_I2C_ADDR, 400000);
    if (!shtc3_handle) return ESP_FAIL;

    esp_err_t err = i2c_master_probe(bus_handle, SHTC3_I2C_ADDR, 200);
    if (err == ESP_OK) {
        uint8_t sensor_id[2];
        shtc3_get_id(shtc3_handle, sensor_id);
        ESP_LOGI(TAG, "SHTC3 ID: 0x%02x%02x", sensor_id[0], sensor_id[1]);
        return ESP_OK;
    }
    return ESP_FAIL;
}

/**************************************************************************************************
 * bk init
 **************************************************************************************************/
static uint8_t brightness = 100;  // 默认亮度100%

esp_err_t bsp_display_brightness_init(void)
{
    return bsp_io_expander_init();
}

esp_err_t bsp_display_brightness_set(int brightness_percent)
{
    if (brightness_percent > 100) brightness_percent = 100;
    if (brightness_percent < 0) brightness_percent = 0;

    brightness = (uint8_t)brightness_percent;
    uint8_t pwm_value = (brightness_percent * 255) / 100;
    
    custom_io_expander_set_pwm(io_expander, pwm_value);
    ESP_LOGI(TAG, "Backlight: %d%%", brightness_percent);
    return ESP_OK;
}

int bsp_display_brightness_get(void)
{
    return (int)brightness;
}

esp_err_t bsp_display_backlight_off(void)
{
    return bsp_display_brightness_set(0);
}

esp_err_t bsp_display_backlight_on(void)
{
    return bsp_display_brightness_set(100);
}


/**************************************************************************************************
 * lcd init
 **************************************************************************************************/
void lcd_fill_screen(esp_lcd_panel_handle_t panel_handle, uint16_t color)
{
    uint32_t pixel_count = BSP_LCD_H_RES * BSP_LCD_V_RES;
    uint8_t *color_buf = heap_caps_malloc(pixel_count * 2, MALLOC_CAP_DMA);
    if (!color_buf) {
        ESP_LOGE("LCD", "malloc failed");
        return;
    }
    uint8_t color_high = (color >> 8) & 0xFF;
    uint8_t color_low = color & 0xFF;
    for (uint32_t i = 0; i < pixel_count; i++) {
        color_buf[i * 2] = color_high;
        color_buf[i * 2 + 1] = color_low;
    }
    esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, BSP_LCD_H_RES, BSP_LCD_V_RES, (uint16_t *)color_buf);
    free(color_buf);
}


esp_err_t bsp_display_new(const bsp_display_config_t *config, esp_lcd_panel_handle_t *ret_panel, esp_lcd_panel_io_handle_t *ret_io)
{
    BSP_ERROR_CHECK_RETURN_ERR(bsp_io_expander_init());

    esp_io_expander_set_level(io_expander, IO_LCD_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(200));
    esp_io_expander_set_level(io_expander, IO_LCD_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(200));

    esp_err_t ret = ESP_OK;
    if(!spi_sd_initialized) bsp_spi_init();
    spi_sd_initialized = true;

    const esp_lcd_panel_io_spi_config_t io_config = {
        .dc_gpio_num = BSP_LCD_DC,
        .cs_gpio_num = BSP_LCD_CS,
        .pclk_hz = 80 * 1000 * 1000,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .spi_mode = 0,
        .trans_queue_depth = 10,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)BSP_LCD_SPI_NUM, &io_config, &io_handle));

    const esp_lcd_panel_dev_config_t panel_config = {
        .reset_gpio_num = BSP_LCD_RST,
        .rgb_ele_order = BSP_LCD_COLOR_SPACE,
        .bits_per_pixel = BSP_LCD_BITS_PER_PIXEL,
        .data_endian = LCD_RGB_DATA_ENDIAN_LITTLE,
    };

    ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(io_handle, &panel_config, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_invert_color(panel_handle, true));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

    if (ret_panel) *ret_panel = panel_handle;
    if (ret_io) *ret_io = io_handle;

    return ret;
}

/**************************************************************************************************
 * Touch init
 **************************************************************************************************/
esp_err_t bsp_touch_new(const bsp_display_cfg_t *cfg, esp_lcd_touch_handle_t *ret_touch)
{

    esp_log_level_set("i2c.master", ESP_LOG_NONE);
    esp_log_level_set("i2c.common", ESP_LOG_NONE);

    BSP_ERROR_CHECK_RETURN_ERR(bsp_i2c_init());
    BSP_ERROR_CHECK_RETURN_ERR(bsp_io_expander_init());

    esp_io_expander_set_level(io_expander, IO_LCD_TOUCH_RST, 0);
    vTaskDelay(pdMS_TO_TICKS(10));
    esp_io_expander_set_level(io_expander, IO_LCD_TOUCH_RST, 1);
    vTaskDelay(pdMS_TO_TICKS(10));

    const esp_lcd_touch_config_t tp_cfg = {
        .x_max = BSP_LCD_H_RES,
        .y_max = BSP_LCD_V_RES,
        .rst_gpio_num = BSP_LCD_TOUCH_RST,
        .int_gpio_num = BSP_LCD_TOUCH_INT,
        .levels = {.reset = 0, .interrupt = 0},
        .flags = {.swap_xy = 0, .mirror_x = 0, .mirror_y = 0},
    };

    esp_lcd_panel_io_handle_t tp_io_handle = NULL;
    esp_lcd_panel_io_i2c_config_t tp_io_config = ESP_LCD_TOUCH_IO_I2C_CST3530_CONFIG();
    tp_io_config.scl_speed_hz = 400000;
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_io_i2c(i2c_handle, &tp_io_config, &tp_io_handle), TAG, "");

    return esp_lcd_touch_new_i2c_cst3530(tp_io_handle, &tp_cfg, ret_touch);
}

/**************************************************************************************************
 * LVGL init
 **************************************************************************************************/
static lv_display_t *bsp_display_lcd_init(const bsp_display_cfg_t *cfg)
{
    assert(cfg != NULL);
    bsp_display_config_t disp_config = {0};
    BSP_ERROR_CHECK_RETURN_NULL(bsp_display_new(&disp_config, &panel_handle, &io_handle));

    esp_lv_adapter_display_config_t disp_cfg = {
        .panel = panel_handle,
        .panel_io = io_handle,
        .profile = {
            .interface = ESP_LV_ADAPTER_PANEL_IF_OTHER,
            .rotation = cfg->rotation,
            .hor_res = BSP_LCD_H_RES,
            .ver_res = BSP_LCD_V_RES,
            .buffer_height = 50,
            .use_psram = true,
            .enable_ppa_accel = false,
            .require_double_buffer = true,
        },
        .tear_avoid_mode = cfg->tear_avoid_mode,
    };
    return esp_lv_adapter_register_display(&disp_cfg);
}

static lv_indev_t *bsp_display_indev_init(const bsp_display_cfg_t *cfg, lv_display_t *disp)
{
    assert(cfg != NULL);
    BSP_ERROR_CHECK_RETURN_NULL(bsp_touch_new(cfg, &tp));
    const esp_lv_adapter_touch_config_t touch_cfg = ESP_LV_ADAPTER_TOUCH_DEFAULT_CONFIG(disp, tp);
    return esp_lv_adapter_register_touch(&touch_cfg);
}

lv_display_t *bsp_display_start(void)
{
    bsp_display_cfg_t cfg = {
        .lv_adapter_cfg = ESP_LV_ADAPTER_DEFAULT_CONFIG(),
        .rotation = ESP_LV_ADAPTER_ROTATE_0,
        .tear_avoid_mode = ESP_LV_ADAPTER_TEAR_AVOID_MODE_NONE,
        .touch_flags = {.swap_xy = 0, .mirror_x = 0, .mirror_y = 0}
    };
    return bsp_display_start_with_config(&cfg);
}

lv_display_t *bsp_display_start_with_config(bsp_display_cfg_t *cfg)
{
    assert(cfg != NULL);
    BSP_ERROR_CHECK_RETURN_NULL(esp_lv_adapter_init(&cfg->lv_adapter_cfg));

    lv_display_t *disp = bsp_display_lcd_init(cfg);
    BSP_NULL_CHECK(disp, NULL);

    disp_indev = bsp_display_indev_init(cfg, disp);
    BSP_NULL_CHECK(disp_indev, NULL);

    BSP_ERROR_CHECK_RETURN_NULL(bsp_io_expander_init());
    BSP_ERROR_CHECK_RETURN_NULL(bsp_display_brightness_init());

    ESP_ERROR_CHECK(esp_lv_adapter_start());
    disp_drv = disp;
    return disp;
}

lv_display_t *bsp_display_get_disp_dev(void)
{
    return disp_drv;
}

lv_indev_t *bsp_display_get_input_dev(void)
{
    return disp_indev;
}

esp_err_t bsp_display_lock(uint32_t timeout_ms)
{
    return esp_lv_adapter_lock(timeout_ms);
}

void bsp_display_unlock(void)
{
    esp_lv_adapter_unlock();
}