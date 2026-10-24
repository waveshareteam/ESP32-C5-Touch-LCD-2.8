/* SPDX-License-Identifier: Apache-2.0 */
#include "bsp/esp-bsp.h"
#include "bsp_board_internal.h"
#include <string.h>
#include "esp_check.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_vfs_fat.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"
#include "nvs_flash.h"
#include "qmi8658.h"
#include "pcf85063a.h"
#include "shtc3.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

static const char *TAG = "bsp_board";
static esp_io_expander_handle_t expander;
static qmi8658_dev_t imu;
static pcf85063a_dev_t rtc;
static i2c_master_dev_handle_t environment;
static bool rtc_ready, imu_ready, environment_ready, wifi_ready, boot_ready, initialized;
static StaticSemaphore_t sensor_storage, sd_storage;
static SemaphoreHandle_t sensor_mutex, sd_mutex;
static sdmmc_card_t *sdcard;
static unsigned sd_leases;
static esp_timer_handle_t boot_timer;
static uint32_t boot_presses;
static portMUX_TYPE boot_lock = portMUX_INITIALIZER_UNLOCKED;
static bsp_init_status_t status = {
    ESP_ERR_INVALID_STATE, ESP_ERR_INVALID_STATE, ESP_ERR_INVALID_STATE,
    ESP_ERR_INVALID_STATE, ESP_ERR_INVALID_STATE, ESP_ERR_INVALID_STATE,
    ESP_ERR_INVALID_STATE, ESP_ERR_INVALID_STATE, ESP_ERR_INVALID_STATE,
    ESP_ERR_INVALID_STATE, ESP_ERR_INVALID_STATE, ESP_ERR_INVALID_STATE,
};

static esp_err_t init_locks(void)
{
    if (!sensor_mutex) sensor_mutex = xSemaphoreCreateMutexStatic(&sensor_storage);
    if (!sd_mutex) sd_mutex = xSemaphoreCreateMutexStatic(&sd_storage);
    return sensor_mutex && sd_mutex ? ESP_OK : ESP_ERR_NO_MEM;
}
const bsp_init_status_t *bsp_get_init_status(void) { return &status; }
bool bsp_board_i2c_in_use(void) { return expander || rtc.dev_handle || imu.dev_handle || environment; }

/**************************************************************************************************
 * CH32V003 IO expander / power amplifier
 **************************************************************************************************/
esp_err_t bsp_io_expander_init(void)
{
    if (expander) return ESP_OK;
    ESP_RETURN_ON_ERROR(bsp_i2c_init(), TAG, "I2C");
    ESP_RETURN_ON_ERROR(custom_io_expander_new_i2c_ch32v003(bsp_i2c_get_handle(),
                        BSP_IO_EXPANDER_I2C_ADDRESS, &expander), TAG, "CH32V003");
    return ESP_OK;
}
esp_io_expander_handle_t bsp_get_io_expander_handle(void) { return expander; }

esp_err_t bsp_battery_get_voltage(float *voltage, uint16_t *raw)
{
    if (!voltage || !raw) return ESP_ERR_INVALID_ARG;
    ESP_RETURN_ON_ERROR(bsp_io_expander_init(), TAG, "Battery ADC");
    uint32_t sum = 0;
    for (unsigned i = 0; i < 8; ++i) {
        uint16_t sample = 0;
        ESP_RETURN_ON_ERROR(custom_io_expander_get_adc(expander, &sample), TAG, "ADC read");
        if (sample > 1023) return ESP_ERR_INVALID_RESPONSE;
        sum += sample;
        vTaskDelay(pdMS_TO_TICKS(5));
    }
    *raw = (sum + 4) / 8;
    *voltage = *raw * 3.3f / 1023.0f * 3.0f;
    return ESP_OK;
}

/**************************************************************************************************
 * BOOT button: GPIO28, active low, no PWR input
 **************************************************************************************************/
static void sample_boot(void *arg)
{
    (void)arg;
    static int candidate = -1, stable = -1;
    static unsigned samples;
    static bool released;
    int raw = gpio_get_level(BSP_BUTTON_BOOT) == BSP_BUTTON_BOOT_ACTIVE_LEVEL;
    if (raw != candidate) { candidate = raw; samples = 1; }
    else if (samples < 3) ++samples;
    if (samples < 3 || raw == stable) return;
    stable = raw;
    if (!raw) released = true;
    else if (released) {
        released = false;
        portENTER_CRITICAL(&boot_lock); ++boot_presses; portEXIT_CRITICAL(&boot_lock);
    }
}
esp_err_t bsp_user_button_init(void)
{
    if (boot_ready) return ESP_OK;
    const gpio_config_t cfg = {
        .pin_bit_mask = 1ULL << BSP_BUTTON_BOOT, .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE, .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    ESP_RETURN_ON_ERROR(gpio_config(&cfg), TAG, "BOOT GPIO28");
    const esp_timer_create_args_t args = {.callback = sample_boot, .name = "boot_button"};
    ESP_RETURN_ON_ERROR(esp_timer_create(&args, &boot_timer), TAG, "BOOT timer");
    esp_err_t ret = esp_timer_start_periodic(boot_timer, 20000);
    if (ret == ESP_OK) boot_ready = true;
    else { esp_timer_delete(boot_timer); boot_timer = NULL; }
    return ret;
}
esp_err_t bsp_button_get_pressed(bsp_button_id_t button, bool *pressed)
{
    if (!pressed || button != BSP_BUTTON_ID_BOOT) return ESP_ERR_INVALID_ARG;
    if (!boot_ready) return ESP_ERR_INVALID_STATE;
    *pressed = gpio_get_level(BSP_BUTTON_BOOT) == BSP_BUTTON_BOOT_ACTIVE_LEVEL;
    return ESP_OK;
}
uint32_t bsp_user_button_get_press_count(void)
{
    portENTER_CRITICAL(&boot_lock); uint32_t n = boot_presses; portEXIT_CRITICAL(&boot_lock);
    return n;
}

/**************************************************************************************************
 * RTC / IMU / environment. Drivers and configuration belong only to BSP.
 **************************************************************************************************/
static bool valid_date(const bsp_sensor_datetime_t *d)
{
    if (!d || d->year < 1970 || d->year > 2069 || d->month < 1 || d->month > 12 ||
        d->hour > 23 || d->minute > 59 || d->second > 59 || d->weekday > 6) return false;
    const unsigned days[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    unsigned max = days[d->month - 1];
    if (d->month == 2 && d->year % 4 == 0 && (d->year % 100 != 0 || d->year % 400 == 0)) ++max;
    return d->day >= 1 && d->day <= max;
}
esp_err_t bsp_rtc_init(void)
{
    if (rtc_ready) return ESP_OK;
    ESP_RETURN_ON_ERROR(bsp_i2c_init(), TAG, "RTC I2C");
    ESP_RETURN_ON_ERROR(init_locks(), TAG, "Board locks");
    xSemaphoreTake(sensor_mutex, portMAX_DELAY);
    esp_err_t ret = pcf85063a_init(&rtc, bsp_i2c_get_handle(), PCF85063A_ADDRESS);
    if (ret == ESP_OK) {
        pcf85063a_datetime_t value = {0};
        uint8_t seconds = 0;
        ret = pcf85063a_read_register(&rtc, PCF85063A_RTC_SECOND_ADDR, &seconds, 1);
        if (ret == ESP_OK) ret = pcf85063a_get_time_date(&rtc, &value);
        bsp_sensor_datetime_t date = {value.year,value.month,value.day,value.hour,value.min,value.sec,value.dotw};
        if (ret == ESP_OK && ((seconds & 0x80) || !valid_date(&date))) {
            value = (pcf85063a_datetime_t){.year=2025,.month=1,.day=1,.dotw=3,.hour=12};
            ret = pcf85063a_set_time_date(&rtc, value);
            ESP_LOGW(TAG, "RTC calendar invalid; set fallback 2025-01-01 12:00");
        }
    }
    rtc_ready = ret == ESP_OK;
    if (!rtc_ready && rtc.dev_handle) { i2c_master_bus_rm_device(rtc.dev_handle); memset(&rtc,0,sizeof(rtc)); }
    xSemaphoreGive(sensor_mutex);
    return ret;
}
esp_err_t bsp_rtc_get_datetime(bsp_sensor_datetime_t *date)
{
    if (!date) return ESP_ERR_INVALID_ARG;
    if (!rtc_ready) return ESP_ERR_INVALID_STATE;
    xSemaphoreTake(sensor_mutex, portMAX_DELAY);
    pcf85063a_datetime_t v = {0};
    esp_err_t ret = pcf85063a_get_time_date(&rtc, &v);
    if (ret == ESP_OK) {
        *date = (bsp_sensor_datetime_t){v.year,v.month,v.day,v.hour,v.min,v.sec,v.dotw};
        if (!valid_date(date)) ret = ESP_ERR_INVALID_RESPONSE;
    }
    xSemaphoreGive(sensor_mutex);
    return ret;
}
esp_err_t bsp_rtc_set_datetime(const bsp_sensor_datetime_t *date)
{
    if (!valid_date(date)) return ESP_ERR_INVALID_ARG;
    if (!rtc_ready) return ESP_ERR_INVALID_STATE;
    pcf85063a_datetime_t v = {.year=date->year,.month=date->month,.day=date->day,.dotw=date->weekday,
                             .hour=date->hour,.min=date->minute,.sec=date->second};
    xSemaphoreTake(sensor_mutex, portMAX_DELAY);
    esp_err_t ret = pcf85063a_set_time_date(&rtc, v);
    xSemaphoreGive(sensor_mutex); return ret;
}
esp_err_t bsp_imu_init(void)
{
    if (imu_ready) return ESP_OK;
    ESP_RETURN_ON_ERROR(bsp_i2c_init(), TAG, "IMU I2C");
    ESP_RETURN_ON_ERROR(init_locks(), TAG, "Board locks");
    xSemaphoreTake(sensor_mutex, portMAX_DELAY);
    esp_err_t ret = qmi8658_init(&imu, bsp_i2c_get_handle(), QMI8658_ADDRESS_HIGH);
    if (ret == ESP_OK) ret = qmi8658_set_accel_odr(&imu, QMI8658_ACCEL_ODR_500HZ);
    if (ret == ESP_OK) ret = qmi8658_write_register(&imu, QMI8658_CTRL5, 0x03);
    qmi8658_set_accel_unit_mps2(&imu, true);
    qmi8658_set_gyro_unit_rads(&imu, false);
    imu_ready = ret == ESP_OK;
    if (!imu_ready && imu.dev_handle) { i2c_master_bus_rm_device(imu.dev_handle); memset(&imu,0,sizeof(imu)); }
    xSemaphoreGive(sensor_mutex); return ret;
}
esp_err_t bsp_imu_get_data(bsp_imu_data_t *data)
{
    if (!data) return ESP_ERR_INVALID_ARG;
    if (!imu_ready) return ESP_ERR_INVALID_STATE;
    xSemaphoreTake(sensor_mutex, portMAX_DELAY);
    qmi8658_data_t v = {0};
    esp_err_t ret = qmi8658_read_sensor_data(&imu, &v);
    if (ret == ESP_OK) *data = (bsp_imu_data_t){v.accelX/9.807f,v.accelY/9.807f,v.accelZ/9.807f,
                                              v.gyroX,v.gyroY,v.gyroZ,v.temperature};
    xSemaphoreGive(sensor_mutex); return ret;
}
esp_err_t bsp_environment_init(void)
{
    if (environment_ready) return ESP_OK;
    ESP_RETURN_ON_ERROR(bsp_i2c_init(), TAG, "Environment I2C");
    ESP_RETURN_ON_ERROR(init_locks(), TAG, "Board locks");
    xSemaphoreTake(sensor_mutex, portMAX_DELAY);
    esp_err_t ret = i2c_master_probe(bsp_i2c_get_handle(), SHTC3_I2C_ADDR, 100);
    if (ret == ESP_OK && !environment) environment = shtc3_device_create(bsp_i2c_get_handle(), SHTC3_I2C_ADDR, BSP_I2C_FREQ_HZ);
    if (ret == ESP_OK && !environment) ret = ESP_ERR_NO_MEM;
    environment_ready = ret == ESP_OK;
    xSemaphoreGive(sensor_mutex); return ret;
}
static uint8_t crc8(const uint8_t *data)
{
    uint8_t crc = 0xff;
    for (int i=0;i<2;++i) { crc ^= data[i]; for (int j=0;j<8;++j) crc = (crc & 0x80) ? (crc << 1) ^ 0x31 : crc << 1; }
    return crc;
}
esp_err_t bsp_environment_get_data(float *temperature, float *humidity)
{
    if (!temperature || !humidity) return ESP_ERR_INVALID_ARG;
    if (!environment_ready) return ESP_ERR_INVALID_STATE;
    xSemaphoreTake(sensor_mutex, portMAX_DELAY);
    uint8_t wake[] = {0x35,0x17}, measure[] = {0x78,0x66}, sleep[] = {0xb0,0x98}, bytes[6];
    esp_err_t ret = i2c_master_transmit(environment, wake, 2, 100);
    if (ret == ESP_OK) { vTaskDelay(pdMS_TO_TICKS(1)); ret = i2c_master_transmit(environment,measure,2,100); }
    if (ret == ESP_OK) { vTaskDelay(pdMS_TO_TICKS(15)); ret = i2c_master_receive(environment,bytes,6,100); }
    if (ret == ESP_OK && (crc8(bytes)!=bytes[2] || crc8(bytes+3)!=bytes[5])) ret=ESP_ERR_INVALID_CRC;
    if (ret == ESP_OK) { *temperature=((bytes[0]*256+bytes[1])*175.0f/65535)-45; *humidity=(bytes[3]*256+bytes[4])*100.0f/65535; }
    i2c_master_transmit(environment,sleep,2,100);
    xSemaphoreGive(sensor_mutex); return ret;
}

/**************************************************************************************************
 * SD card. Leases prevent unmount while a media APP owns open files.
 **************************************************************************************************/
static esp_err_t mount_sd_locked(void)
{
    if (sdcard) return ESP_OK;
    ESP_RETURN_ON_ERROR(bsp_spi_bus_init(), TAG, "Shared SPI");
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    host.slot = BSP_LCD_SPI_HOST;
    host.max_freq_khz = 20000;
    sdspi_device_config_t slot = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot.host_id = BSP_LCD_SPI_HOST; slot.gpio_cs = BSP_SD_SDCS;
    const esp_vfs_fat_sdmmc_mount_config_t cfg = {
        .format_if_mount_failed = false, .max_files = 8, .allocation_unit_size = 16*1024,
    };
    return esp_vfs_fat_sdspi_mount(BSP_SD_MOUNT_POINT, &host, &slot, &cfg, &sdcard);
}
esp_err_t bsp_sdcard_mount(void)
{
    ESP_RETURN_ON_ERROR(init_locks(), TAG, "Board locks");
    xSemaphoreTake(sd_mutex, portMAX_DELAY);
    esp_err_t ret = mount_sd_locked();
    xSemaphoreGive(sd_mutex); return ret;
}
esp_err_t bsp_sdcard_acquire(void)
{
    ESP_RETURN_ON_ERROR(init_locks(), TAG, "Board locks");
    xSemaphoreTake(sd_mutex, portMAX_DELAY);
    esp_err_t ret = mount_sd_locked();
    if (ret == ESP_OK) ++sd_leases;
    xSemaphoreGive(sd_mutex); return ret;
}
void bsp_sdcard_release(void)
{
    if (!sd_mutex) return;
    xSemaphoreTake(sd_mutex, portMAX_DELAY); if (sd_leases) --sd_leases; xSemaphoreGive(sd_mutex);
}
esp_err_t bsp_sdcard_unmount(void)
{
    if (!sd_mutex) return ESP_ERR_INVALID_STATE;
    xSemaphoreTake(sd_mutex, portMAX_DELAY);
    esp_err_t ret = sd_leases ? ESP_ERR_INVALID_STATE : ESP_OK;
    if (ret == ESP_OK && sdcard) {
        ret = esp_vfs_fat_sdcard_unmount(BSP_SD_MOUNT_POINT, sdcard);
        if (ret == ESP_OK) sdcard = NULL;
    }
    // The SPI host belongs to both LCD and SD; never free it here.
    xSemaphoreGive(sd_mutex); return ret;
}
esp_err_t bsp_sdcard_get_info(bsp_sdcard_info_t *info)
{
    if (!info) return ESP_ERR_INVALID_ARG;
    *info = (bsp_sdcard_info_t){0};
    if (!sd_mutex) return ESP_ERR_INVALID_STATE;
    xSemaphoreTake(sd_mutex, portMAX_DELAY);
    esp_err_t ret = ESP_ERR_INVALID_STATE;
    if (sdcard) {
        info->mounted = true;
        info->capacity_bytes = (uint64_t)sdcard->csd.capacity * sdcard->csd.sector_size;
        ret = esp_vfs_fat_info(BSP_SD_MOUNT_POINT, &info->total_bytes, &info->free_bytes);
    }
    xSemaphoreGive(sd_mutex); return ret;
}

/**************************************************************************************************
 * Wi-Fi driver / board startup
 **************************************************************************************************/
esp_err_t bsp_wifi_init(void)
{
    if (wifi_ready) return ESP_OK;
    ESP_RETURN_ON_ERROR(esp_netif_init(), TAG, "Netif");
    esp_err_t ret = esp_event_loop_create_default();
    if (ret != ESP_OK && ret != ESP_ERR_INVALID_STATE) return ret;
    if (!esp_netif_create_default_wifi_sta() || !esp_netif_create_default_wifi_ap()) return ESP_ERR_NO_MEM;
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_RETURN_ON_ERROR(esp_wifi_init(&cfg), TAG, "Wi-Fi driver");
    ret = esp_wifi_set_mode(WIFI_MODE_STA);
    if (ret == ESP_OK) wifi_ready = true;
    else esp_wifi_deinit();
    return ret;
}
esp_err_t bsp_wifi_start(void)
{
    if (!wifi_ready) return ESP_ERR_INVALID_STATE;
    ESP_RETURN_ON_ERROR(esp_wifi_start(), TAG, "Wi-Fi start");
    return esp_wifi_set_band_mode(WIFI_BAND_MODE_2G_ONLY);
}
static void report(const char *name, esp_err_t ret)
{
    if (ret == ESP_OK) ESP_LOGI(TAG, "%s ready", name);
    else ESP_LOGW(TAG, "%s unavailable: %s", name, esp_err_to_name(ret));
}
esp_err_t bsp_init(void)
{
    if (initialized) return ESP_OK;
    ESP_RETURN_ON_ERROR(init_locks(), TAG, "Board locks");
    status.i2c = bsp_i2c_init();
    ESP_RETURN_ON_ERROR(status.i2c, TAG, "I2C");
    status.expander = bsp_io_expander_init();
    ESP_RETURN_ON_ERROR(status.expander, TAG, "CH32V003");
    status.power = bsp_display_brightness_init();
    ESP_RETURN_ON_ERROR(status.power, TAG, "Backlight off");
    ESP_RETURN_ON_ERROR(nvs_flash_init(), TAG, "NVS (saved data preserved)");
    // Put the SD card in SPI mode before the LCD begins shared-bus traffic.
    status.sdcard = bsp_sdcard_mount(); report("SD card", status.sdcard);
    status.display = bsp_display_start() ? ESP_OK : ESP_FAIL;
    status.touch = bsp_touch_get_handle() ? ESP_OK : ESP_FAIL;
    ESP_RETURN_ON_ERROR(status.display, TAG, "LCD/LVGL");
    ESP_RETURN_ON_ERROR(status.touch, TAG, "CST3530");
    status.button = bsp_user_button_init(); report("BOOT GPIO28", status.button);
    status.audio = bsp_board_pa_set(false);
    if (status.audio == ESP_OK) status.audio = bsp_audio_codec_speaker_init() ? ESP_OK : ESP_FAIL;
    report("ES8311", status.audio);
    status.rtc = bsp_rtc_init(); report("PCF85063A", status.rtc);
    status.imu = bsp_imu_init(); report("QMI8658", status.imu);
    status.environment = bsp_environment_init(); report("SHTC3", status.environment);
    status.wifi = bsp_wifi_init(); report("Wi-Fi driver", status.wifi);
    initialized = true;
    return ESP_OK;
}
