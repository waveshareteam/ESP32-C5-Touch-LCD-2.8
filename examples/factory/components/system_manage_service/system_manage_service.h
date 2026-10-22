#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "bsp/esp-bsp.h"
#include "wifi_manager/wifi_manager.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef esp_err_t (*sys_manage_set_percent_cb_t)(uint16_t value);
typedef uint16_t (*sys_manage_get_percent_cb_t)(void);

typedef struct {
    const char *device_name;
    const char *firmware_name;

    uint16_t backlight_value;
    bool wifi_connected;
    char wifi_ssid[33];
    wifi_mgr_mode_t wifi_mode;

    sys_manage_set_percent_cb_t set_backlight;
    sys_manage_get_percent_cb_t get_backlight;

    esp_err_t (*scan_wifi)(wifi_ap_record_t *records, uint16_t *record_count);
    esp_err_t (*start_wifi_provisioning)(void);
    esp_err_t (*stop_wifi_provisioning)(bool restart_sta);
    void (*get_wifi_provisioning_ssid)(char *ssid, size_t ssid_size);
} sys_manage_service_t;

sys_manage_service_t *system_manage_service_init(void);
sys_manage_service_t *get_system_manage_service_handle(void);

esp_err_t system_manage_service_set_backlight(uint16_t value);
esp_err_t system_manage_service_scan_wifi(wifi_ap_record_t *records, uint16_t *record_count);
esp_err_t system_manage_service_start_wifi_provisioning(void);
esp_err_t system_manage_service_stop_wifi_provisioning(bool restart_sta);
void system_manage_service_get_wifi_provisioning_ssid(char *ssid, size_t ssid_size);

#ifdef __cplusplus
}
#endif
