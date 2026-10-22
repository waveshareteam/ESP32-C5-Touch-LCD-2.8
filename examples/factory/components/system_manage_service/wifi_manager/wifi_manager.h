#pragma once

#include <stdbool.h>
#include <stddef.h>
#include "esp_err.h"
#include "esp_wifi.h"
#include "esp_event.h"

#define WIFI_MGR_MAX_CALLBACKS 8


typedef enum {
    WIFI_MGR_MODE_NONE = 0,
    WIFI_MGR_MODE_STA,
    WIFI_MGR_MODE_AP,
} wifi_mgr_mode_t;

typedef enum {
    WIFI_MGR_EVENT_CONNECTED,
    WIFI_MGR_EVENT_DISCONNECTED,
    WIFI_MGR_EVENT_FAIL,
    WIFI_MGR_EVENT_AP_STA_CONNECTED,
    WIFI_MGR_EVENT_AP_STA_DISCONNECTED,
    WIFI_MGR_EVENT_PROVISIONING_STARTED,
    WIFI_MGR_EVENT_PROVISIONING_STOPPED,
} wifi_mgr_event_t;

typedef void (*wifi_mgr_callback_t)(wifi_mgr_event_t event, void *event_data);

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t wifi_manager_init(void);

esp_err_t wifi_manager_register_callback(wifi_mgr_callback_t cb);
esp_err_t wifi_manager_unregister_callback(wifi_mgr_callback_t cb);

esp_err_t wifi_manager_start_sta(const char *ssid, const char *password);
esp_err_t wifi_manager_start_ap(const char *ssid, const char *password,
                                 uint8_t channel, uint8_t max_conn);
esp_err_t wifi_manager_start_provisioning(void);
esp_err_t wifi_manager_stop_provisioning(bool restart_sta);
esp_err_t wifi_manager_stop(void);

bool             wifi_manager_is_connected(void);
wifi_mgr_mode_t  wifi_manager_get_mode(void);
esp_err_t        wifi_manager_prepare_scan(void);
esp_err_t        wifi_manager_finish_scan(void);
void             wifi_manager_get_provisioning_ssid(char *ssid, size_t ssid_size);
void             wifi_manager_get_sta_ssid(char *ssid, size_t ssid_size);

#ifdef __cplusplus
}
#endif
