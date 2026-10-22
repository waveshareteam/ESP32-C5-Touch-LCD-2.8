#include "system_manage_service.h"

#include <stdlib.h>
#include <string.h>

#include "esp_check.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "system_manage_service";

#define WIFI_SCAN_STATE_RETRY_COUNT 10
#define WIFI_SCAN_STATE_RETRY_DELAY_MS 200

static sys_manage_service_t *sys_manage_service = NULL;

static uint16_t clamp_percent(uint16_t value)
{
    return value > 100 ? 100 : value;
}

static esp_err_t default_set_backlight(uint16_t value)
{
    return bsp_display_brightness_set(clamp_percent(value));
}

static uint16_t default_get_backlight(void)
{
    if (sys_manage_service == NULL) {
        return 100;
    }
    return sys_manage_service->backlight_value;
}

static void wifi_event_cb(wifi_mgr_event_t event, void *event_data)
{
    (void)event_data;

    if (sys_manage_service == NULL) {
        return;
    }

    sys_manage_service->wifi_mode = wifi_manager_get_mode();
    sys_manage_service->wifi_connected = wifi_manager_is_connected();

    if (sys_manage_service->wifi_connected) {
        wifi_manager_get_sta_ssid(sys_manage_service->wifi_ssid, sizeof(sys_manage_service->wifi_ssid));
    } else {
        sys_manage_service->wifi_ssid[0] = '\0';
    }

    switch (event) {
        case WIFI_MGR_EVENT_CONNECTED:
            ESP_LOGI(TAG, "WiFi connected: %s", sys_manage_service->wifi_ssid);
            break;
        case WIFI_MGR_EVENT_DISCONNECTED:
            ESP_LOGW(TAG, "WiFi disconnected");
            break;
        case WIFI_MGR_EVENT_FAIL:
            ESP_LOGE(TAG, "WiFi connection failed");
            break;
        case WIFI_MGR_EVENT_PROVISIONING_STARTED:
            ESP_LOGI(TAG, "WiFi provisioning started");
            break;
        case WIFI_MGR_EVENT_PROVISIONING_STOPPED:
            ESP_LOGI(TAG, "WiFi provisioning stopped");
            break;
        default:
            break;
    }
}

sys_manage_service_t *system_manage_service_init(void)
{
    if (sys_manage_service != NULL) {
        return sys_manage_service;
    }

    sys_manage_service = (sys_manage_service_t *)calloc(1, sizeof(sys_manage_service_t));
    ESP_RETURN_ON_FALSE(sys_manage_service != NULL, NULL, TAG, "no memory for system manage service");

    sys_manage_service->device_name = "esp32_c5_touch_lcd_2_8";
    sys_manage_service->firmware_name = "Brookesia Phone Demo";
    sys_manage_service->backlight_value = 100;
    sys_manage_service->wifi_mode = WIFI_MGR_MODE_NONE;
    sys_manage_service->wifi_ssid[0] = '\0';
    sys_manage_service->set_backlight = default_set_backlight;
    sys_manage_service->get_backlight = default_get_backlight;
    sys_manage_service->scan_wifi = system_manage_service_scan_wifi;
    sys_manage_service->start_wifi_provisioning = system_manage_service_start_wifi_provisioning;
    sys_manage_service->stop_wifi_provisioning = system_manage_service_stop_wifi_provisioning;
    sys_manage_service->get_wifi_provisioning_ssid = system_manage_service_get_wifi_provisioning_ssid;

    ESP_ERROR_CHECK(wifi_manager_init());
    ESP_ERROR_CHECK(wifi_manager_register_callback(wifi_event_cb));
    ESP_ERROR_CHECK(wifi_manager_start_sta("weather-wifi", "12345678"));

    sys_manage_service->wifi_mode = wifi_manager_get_mode();
    return sys_manage_service;
}

sys_manage_service_t *get_system_manage_service_handle(void)
{
    return sys_manage_service;
}

esp_err_t system_manage_service_set_backlight(uint16_t value)
{
    ESP_RETURN_ON_FALSE(sys_manage_service != NULL, ESP_ERR_INVALID_STATE, TAG, "service not initialized");
    ESP_RETURN_ON_FALSE(sys_manage_service->set_backlight != NULL, ESP_ERR_INVALID_STATE, TAG, "backlight setter missing");

    value = clamp_percent(value);
    esp_err_t ret = sys_manage_service->set_backlight(value);
    if (ret == ESP_OK) {
        sys_manage_service->backlight_value = value;
    }
    return ret;
}

esp_err_t system_manage_service_scan_wifi(wifi_ap_record_t *records, uint16_t *record_count)
{
    ESP_RETURN_ON_FALSE(records != NULL, ESP_ERR_INVALID_ARG, TAG, "records is NULL");
    ESP_RETURN_ON_FALSE(record_count != NULL, ESP_ERR_INVALID_ARG, TAG, "record_count is NULL");
    ESP_RETURN_ON_FALSE(*record_count > 0, ESP_ERR_INVALID_ARG, TAG, "record_count is zero");

    esp_err_t ret = wifi_manager_prepare_scan();
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "WiFi scan preparation failed: %s", esp_err_to_name(ret));
        *record_count = 0;
        return ret;
    }

    wifi_scan_config_t scan_config = {0};
    for (int attempt = 0; attempt < WIFI_SCAN_STATE_RETRY_COUNT; attempt++) {
        ret = esp_wifi_scan_start(&scan_config, true);
        if (ret != ESP_ERR_WIFI_STATE) {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(WIFI_SCAN_STATE_RETRY_DELAY_MS));
    }

    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "WiFi scan failed: %s", esp_err_to_name(ret));
        *record_count = 0;
        wifi_manager_finish_scan();
        return ret;
    }

    ret = esp_wifi_scan_get_ap_records(record_count, records);
    esp_err_t resume_ret = wifi_manager_finish_scan();
    if (ret != ESP_OK) {
        return ret;
    }
    return resume_ret == ESP_OK ? ESP_OK : resume_ret;
}

esp_err_t system_manage_service_start_wifi_provisioning(void)
{
    ESP_RETURN_ON_FALSE(sys_manage_service != NULL, ESP_ERR_INVALID_STATE, TAG, "service not initialized");
    return wifi_manager_start_provisioning();
}

esp_err_t system_manage_service_stop_wifi_provisioning(bool restart_sta)
{
    ESP_RETURN_ON_FALSE(sys_manage_service != NULL, ESP_ERR_INVALID_STATE, TAG, "service not initialized");
    return wifi_manager_stop_provisioning(restart_sta);
}

void system_manage_service_get_wifi_provisioning_ssid(char *ssid, size_t ssid_size)
{
    wifi_manager_get_provisioning_ssid(ssid, ssid_size);
}
