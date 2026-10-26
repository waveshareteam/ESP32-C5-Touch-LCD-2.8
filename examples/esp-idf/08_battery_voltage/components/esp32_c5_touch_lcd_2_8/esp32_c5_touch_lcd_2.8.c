/* SPDX-License-Identifier: Apache-2.0 */
#include <assert.h>
#include <string.h>
#include "bsp/esp-bsp.h"
#include "bsp_board_internal.h"
#include "bsp_err_check.h"
#include "esp_check.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_st7789.h"
#include "esp_lcd_touch_cst3530.h"
#include "esp_codec_dev_defaults.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "bsp_c5_2_8";
static i2c_master_bus_handle_t i2c_handle;
static bool i2c_initialized, spi_initialized;
static lv_display_t *disp_drv;
static lv_indev_t *disp_indev;
static esp_lcd_touch_handle_t tp;
static esp_lcd_panel_handle_t panel_handle;
static esp_lcd_panel_io_handle_t io_handle, touch_io;
static i2s_chan_handle_t i2s_tx_chan, i2s_rx_chan;
static const audio_codec_data_if_t *i2s_data_if;
static const audio_codec_ctrl_if_t *audio_ctrl_if;
static const audio_codec_gpio_if_t *audio_gpio_if;
static const audio_codec_if_t *audio_codec_if;
static esp_codec_dev_handle_t audio_codec;
static bool backlight_released;
static int brightness = 100;

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
 * I2C interface
 **************************************************************************************************/
esp_err_t bsp_i2c_init(void)
{
    if (i2c_initialized) return ESP_OK;
    const i2c_master_bus_config_t config = {
        .i2c_port = BSP_I2C_NUM, .sda_io_num = BSP_I2C_SDA, .scl_io_num = BSP_I2C_SCL,
        .clk_source = I2C_CLK_SRC_DEFAULT, .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    esp_err_t ret = i2c_new_master_bus(&config, &i2c_handle);
    i2c_initialized = ret == ESP_OK;
    return ret;
}
esp_err_t bsp_i2c_deinit(void)
{
    if (!i2c_initialized) return ESP_OK;
    if (tp || audio_codec || bsp_board_i2c_in_use()) return ESP_ERR_INVALID_STATE;
    esp_err_t ret = i2c_del_master_bus(i2c_handle);
    if (ret == ESP_OK) { i2c_handle = NULL; i2c_initialized = false; }
    return ret;
}
i2c_master_bus_handle_t bsp_i2c_get_handle(void) { return i2c_handle; }

/**************************************************************************************************
 * Shared SPI bus: LCD and SD own separate devices, never reinitialize the host
 **************************************************************************************************/
esp_err_t bsp_spi_bus_init(void)
{
    if (spi_initialized) return ESP_OK;
    gpio_config_t cs = { .pin_bit_mask = (1ULL << BSP_LCD_CS) | (1ULL << BSP_SD_SDCS),
                         .mode = GPIO_MODE_OUTPUT };
    ESP_RETURN_ON_ERROR(gpio_config(&cs), TAG, "Chip selects");
    gpio_set_level(BSP_LCD_CS, 1); gpio_set_level(BSP_SD_SDCS, 1);
    spi_bus_config_t cfg = {
        .mosi_io_num = BSP_LCD_MOSI, .miso_io_num = BSP_SD_MISO, .sclk_io_num = BSP_LCD_SCLK,
        .quadwp_io_num = -1, .quadhd_io_num = -1,
        .max_transfer_sz = BSP_LCD_H_RES * 20 * 2,
    };
    esp_err_t ret = spi_bus_initialize(BSP_LCD_SPI_HOST, &cfg, SPI_DMA_CH_AUTO);
    spi_initialized = ret == ESP_OK;
    return ret;
}

/**************************************************************************************************
 * I2S audio interface
 **************************************************************************************************/
 esp_err_t bsp_audio_init(const i2s_std_config_t *i2s_config)
{
    esp_err_t ret = ESP_FAIL;
    if (i2s_tx_chan && i2s_rx_chan) {
        return ESP_OK;
    }

    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(BSP_I2S_NUM, I2S_ROLE_MASTER);
    chan_cfg.auto_clear = true;
    chan_cfg.dma_desc_num = BSP_AUDIO_DMA_DESCRIPTORS;
    chan_cfg.dma_frame_num = BSP_AUDIO_DMA_FRAMES;
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
    bsp_audio_deinit();
    return ret;
}

esp_err_t bsp_audio_init_voice_24k(void)
{
    i2s_std_config_t cfg = BSP_I2S_DUPLEX_MONO_CFG(24000);
    cfg.slot_cfg.slot_mode = I2S_SLOT_MODE_STEREO;
    cfg.slot_cfg.slot_mask = I2S_STD_SLOT_BOTH;
    return bsp_audio_init(&cfg);
}

esp_err_t bsp_audio_deinit(void)
{
    if (audio_codec) { esp_codec_dev_delete(audio_codec); audio_codec = NULL; }
    if (audio_codec_if) { audio_codec_delete_codec_if(audio_codec_if); audio_codec_if = NULL; }
    if (audio_ctrl_if) { audio_codec_delete_ctrl_if(audio_ctrl_if); audio_ctrl_if = NULL; }
    if (audio_gpio_if) { audio_codec_delete_gpio_if(audio_gpio_if); audio_gpio_if = NULL; }
    if (i2s_data_if) { audio_codec_delete_data_if(i2s_data_if); i2s_data_if = NULL; }
    if (i2s_tx_chan) { i2s_channel_disable(i2s_tx_chan); i2s_del_channel(i2s_tx_chan); i2s_tx_chan = NULL; }
    if (i2s_rx_chan) { i2s_channel_disable(i2s_rx_chan); i2s_del_channel(i2s_rx_chan); i2s_rx_chan = NULL; }
    return ESP_OK;
}

esp_err_t bsp_audio_i2s_read(void *data, size_t size, size_t *read, uint32_t timeout_ms)
{
    return i2s_rx_chan ? i2s_channel_read(i2s_rx_chan, data, size, read, timeout_ms) : ESP_ERR_INVALID_STATE;
}

esp_err_t bsp_audio_i2s_write(void *data, size_t size, size_t *written, uint32_t timeout_ms)
{
    return i2s_tx_chan ? i2s_channel_write(i2s_tx_chan, data, size, written, timeout_ms) : ESP_ERR_INVALID_STATE;
}

esp_err_t bsp_board_pa_set(bool enabled)
{
    esp_err_t ret = bsp_io_expander_init();
    if (ret != ESP_OK) return ret;
    ESP_RETURN_ON_ERROR(esp_io_expander_set_dir(bsp_get_io_expander_handle(), IO_POWER_AMP_IO, IO_EXPANDER_OUTPUT), TAG, "Amplifier direction");
    return esp_io_expander_set_level(bsp_get_io_expander_handle(), IO_POWER_AMP_IO, enabled ? 1 : 0);
}

const audio_codec_data_if_t *bsp_audio_get_codec_itf(void)
{
    return i2s_data_if;
}



esp_codec_dev_handle_t bsp_audio_codec_speaker_init(void)
{
    if (audio_codec) return audio_codec;
    BSP_ERROR_CHECK_RETURN_NULL(bsp_i2c_init());
    const audio_codec_data_if_t *i2s_data_if = bsp_audio_get_codec_itf();
    if (i2s_data_if == NULL) {
        BSP_ERROR_CHECK_RETURN_NULL(bsp_i2c_init());
        BSP_ERROR_CHECK_RETURN_NULL(bsp_audio_init_voice_24k());
        i2s_data_if = bsp_audio_get_codec_itf();
    }
    assert(i2s_data_if);

    const audio_codec_gpio_if_t *gpio_if = audio_codec_new_gpio();
    audio_gpio_if = gpio_if;
    if (!gpio_if) { bsp_audio_deinit(); return NULL; }
    audio_codec_i2c_cfg_t i2c_cfg = {
        .port = BSP_I2C_NUM,
        .addr = ES8311_CODEC_DEFAULT_ADDR,
        .bus_handle = i2c_handle,
    };
    const audio_codec_ctrl_if_t *i2c_ctrl_if = audio_codec_new_i2c_ctrl(&i2c_cfg);
    audio_ctrl_if = i2c_ctrl_if;
    if (!i2c_ctrl_if) { bsp_audio_deinit(); return NULL; }

    esp_codec_dev_hw_gain_t gain = {
        .pa_voltage = 5.0,
        .codec_dac_voltage = 3.3,
    };

    es8311_codec_cfg_t es8311_cfg = {
        .ctrl_if = i2c_ctrl_if,
        .gpio_if = gpio_if,
        .codec_mode = ESP_CODEC_DEV_WORK_MODE_BOTH,
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
    audio_codec_if = es8311_dev;
    if (!es8311_dev) { bsp_audio_deinit(); return NULL; }

    esp_codec_dev_cfg_t codec_dev_cfg = {
        .dev_type = ESP_CODEC_DEV_TYPE_IN_OUT,
        .codec_if = es8311_dev,
        .data_if = i2s_data_if,
    };
    audio_codec = esp_codec_dev_new(&codec_dev_cfg);
    if (!audio_codec) bsp_audio_deinit();
    return audio_codec;
}

esp_codec_dev_handle_t bsp_audio_codec_microphone_init(void)
{
    return bsp_audio_codec_speaker_init();
}



/**************************************************************************************************
 * LCD / backlight interface
 **************************************************************************************************/
esp_err_t bsp_display_brightness_init(void)
{
    ESP_RETURN_ON_ERROR(bsp_io_expander_init(), TAG, "Backlight controller");
    return custom_io_expander_set_pwm(bsp_get_io_expander_handle(), backlight_released ? brightness * 255 / 100 : 0);
}
esp_err_t bsp_display_brightness_set(int percent)
{
    if (percent < 0 || percent > 100) return ESP_ERR_INVALID_ARG;
    if (!bsp_get_io_expander_handle()) return ESP_ERR_INVALID_STATE;
    esp_err_t ret = custom_io_expander_set_pwm(bsp_get_io_expander_handle(), backlight_released ? percent * 255 / 100 : 0);
    if (ret == ESP_OK) brightness = percent;
    return ret;
}

int bsp_display_brightness_get(void) { return brightness; }
esp_err_t bsp_display_backlight_off(void) { return bsp_display_brightness_set(0); }
esp_err_t bsp_display_backlight_on(void) { return bsp_display_brightness_set(100); }

esp_err_t bsp_display_new(const bsp_display_config_t *config, esp_lcd_panel_handle_t *panel, esp_lcd_panel_io_handle_t *io)
{
    (void)config;
    if (!panel || !io) return ESP_ERR_INVALID_ARG;
    if (panel_handle) { *panel = panel_handle; *io = io_handle; return ESP_OK; }
    ESP_RETURN_ON_ERROR(bsp_display_brightness_init(), TAG, "Backlight off");
    ESP_RETURN_ON_ERROR(bsp_spi_bus_init(), TAG, "LCD SPI");
    esp_io_expander_handle_t expander = bsp_get_io_expander_handle();
    ESP_RETURN_ON_ERROR(esp_io_expander_set_dir(expander, IO_LCD_RST, IO_EXPANDER_OUTPUT), TAG, "LCD reset direction");
    ESP_RETURN_ON_ERROR(esp_io_expander_set_level(expander, IO_LCD_RST, 0), TAG, "LCD reset");
    vTaskDelay(pdMS_TO_TICKS(200));
    ESP_RETURN_ON_ERROR(esp_io_expander_set_level(expander, IO_LCD_RST, 1), TAG, "LCD reset");
    vTaskDelay(pdMS_TO_TICKS(200));
    const esp_lcd_panel_io_spi_config_t iocfg = {
        .dc_gpio_num = BSP_LCD_DC, .cs_gpio_num = BSP_LCD_CS, .pclk_hz = BSP_LCD_PIXEL_CLOCK_HZ,
        .lcd_cmd_bits = 8, .lcd_param_bits = 8, .spi_mode = 0, .trans_queue_depth = 3,
    };
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)BSP_LCD_SPI_HOST, &iocfg, &io_handle), TAG, "LCD IO");
    const esp_lcd_panel_dev_config_t cfg = {
        .reset_gpio_num = BSP_LCD_RST, .rgb_ele_order = BSP_LCD_COLOR_SPACE, .bits_per_pixel = 16,
    };
    esp_err_t ret = esp_lcd_new_panel_st7789(io_handle, &cfg, &panel_handle);
    if (ret == ESP_OK) ret = esp_lcd_panel_init(panel_handle);
    if (ret == ESP_OK) ret = esp_lcd_panel_invert_color(panel_handle, true);
    if (ret == ESP_OK) ret = esp_lcd_panel_disp_on_off(panel_handle, true);
    if (ret != ESP_OK) {
        if (panel_handle) esp_lcd_panel_del(panel_handle);
        esp_lcd_panel_io_del(io_handle); panel_handle = NULL; io_handle = NULL;
        return ret;
    }
    *panel = panel_handle; *io = io_handle;
    return ESP_OK;
}

/**************************************************************************************************
 * CST3530 touch / LVGL adapter porting
 **************************************************************************************************/
static esp_err_t touch_init(const bsp_display_cfg_t *cfg)
{
    if (tp) return ESP_OK;
    esp_io_expander_handle_t expander = bsp_get_io_expander_handle();
    ESP_RETURN_ON_ERROR(esp_io_expander_set_dir(expander, IO_LCD_TOUCH_RST, IO_EXPANDER_OUTPUT), TAG, "Touch reset direction");
    ESP_RETURN_ON_ERROR(esp_io_expander_set_level(expander, IO_LCD_TOUCH_RST, 0), TAG, "Touch reset");
    vTaskDelay(pdMS_TO_TICKS(10));
    ESP_RETURN_ON_ERROR(esp_io_expander_set_level(expander, IO_LCD_TOUCH_RST, 1), TAG, "Touch reset");
    vTaskDelay(pdMS_TO_TICKS(10));
    esp_lcd_panel_io_i2c_config_t io = ESP_LCD_TOUCH_IO_I2C_CST3530_CONFIG();
    io.scl_speed_hz = BSP_I2C_FREQ_HZ;
    ESP_RETURN_ON_ERROR(esp_lcd_new_panel_io_i2c(i2c_handle, &io, &touch_io), TAG, "Touch IO");
    const esp_lcd_touch_config_t config = {
        .x_max = BSP_LCD_H_RES, .y_max = BSP_LCD_V_RES,
        .rst_gpio_num = BSP_TOUCH_RST, .int_gpio_num = BSP_TOUCH_INT,
        .levels = {.reset = 0, .interrupt = 0},
        .flags = {.swap_xy = cfg->touch_flags.swap_xy, .mirror_x = cfg->touch_flags.mirror_x, .mirror_y = cfg->touch_flags.mirror_y},
    };
    esp_err_t ret = esp_lcd_touch_new_i2c_cst3530(touch_io, &config, &tp);
    if (ret != ESP_OK) { esp_lcd_panel_io_del(touch_io); touch_io = NULL; }
    return ret;
}
lv_display_t *bsp_display_start(void)
{
    bsp_display_cfg_t cfg = {
        .lv_adapter_cfg = ESP_LV_ADAPTER_DEFAULT_CONFIG(),
        .rotation = ESP_LV_ADAPTER_ROTATE_0,
        .tear_avoid_mode = ESP_LV_ADAPTER_TEAR_AVOID_MODE_NONE,
    };
    return bsp_display_start_with_config(&cfg);
}
lv_display_t *bsp_display_start_with_config(const bsp_display_cfg_t *cfg)
{
    if (disp_drv) return disp_drv;
    if (!cfg || esp_lv_adapter_init(&cfg->lv_adapter_cfg) != ESP_OK) return NULL;
    bsp_display_config_t lcd = {0};
    if (bsp_display_new(&lcd, &panel_handle, &io_handle) != ESP_OK) goto failed;
    const esp_lv_adapter_display_config_t display = {
        .panel = panel_handle, .panel_io = io_handle,
        .profile = {
            .interface = ESP_LV_ADAPTER_PANEL_IF_OTHER, .rotation = cfg->rotation,
            .hor_res = BSP_LCD_H_RES, .ver_res = BSP_LCD_V_RES,
            .buffer_height = 20, .use_psram = false,
            .enable_ppa_accel = false, .require_double_buffer = true,
        },
        .tear_avoid_mode = cfg->tear_avoid_mode,
    };
    disp_drv = esp_lv_adapter_register_display(&display);
    if (!disp_drv) goto failed;
    if (touch_init(cfg) != ESP_OK) goto failed;
    const esp_lv_adapter_touch_config_t touch = ESP_LV_ADAPTER_TOUCH_DEFAULT_CONFIG(disp_drv, tp);
    disp_indev = esp_lv_adapter_register_touch(&touch);
    if (!disp_indev || esp_lv_adapter_start() != ESP_OK) goto failed;
    ESP_LOGI(TAG, "ST7789 240x320; CST3530; LVGL adapter; 2 x 9600-byte internal DMA buffers");
    return disp_drv;
failed:
    // Do not expose a partially registered display as a successful retry.
    if (esp_lv_adapter_deinit() != ESP_OK) ESP_LOGE(TAG, "LVGL startup cleanup failed");
    disp_drv = NULL; disp_indev = NULL;
    return NULL;
}
lv_display_t *bsp_display_get(void) { return disp_drv; }
lv_display_t *bsp_display_get_disp_dev(void) { return disp_drv; }
lv_indev_t *bsp_display_get_input_dev(void) { return disp_indev; }
esp_lcd_touch_handle_t bsp_touch_get_handle(void) { return tp; }
bool bsp_display_lock(uint32_t timeout_ms) { return esp_lv_adapter_lock(timeout_ms) == ESP_OK; }
void bsp_display_unlock(void) { esp_lv_adapter_unlock(); }
esp_err_t bsp_display_show(void)
{
    if (!disp_drv || !io_handle) return ESP_ERR_INVALID_STATE;
    // The refresh task may own the lock after UI creation; wait for it.
    if (!bsp_display_lock(UINT32_MAX)) return ESP_ERR_TIMEOUT;
    lv_obj_invalidate(lv_display_get_screen_active(disp_drv));
    lv_refr_now(disp_drv);
    esp_err_t ret = esp_lcd_panel_io_tx_param(io_handle, -1, NULL, 0);
    if (ret == ESP_OK) {
        backlight_released = true;
        ret = bsp_display_brightness_set(brightness);
        if (ret != ESP_OK) backlight_released = false;
    }
    bsp_display_unlock();
    if (ret == ESP_OK) ESP_LOGI(TAG, "Initial UI flushed; backlight enabled at %d%%", brightness);
    return ret;
}
