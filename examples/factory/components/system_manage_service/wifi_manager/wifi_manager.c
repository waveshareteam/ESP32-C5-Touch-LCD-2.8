#include "wifi_manager.h"

#include <string.h>
#include <sys/param.h>
#include "cJSON.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "dns_server.h"
#include "esp_check.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "lwip/inet.h"
#include "lwip/err.h"
#include "lwip/sys.h"

#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1
#define WIFI_MAX_RETRY     3
#define WIFI_RECONNECT_BACKOFF_MS 10000
#define WIFI_PROV_AP_PASSWORD "12345678"
#define WIFI_PROV_TASK_STACK 6144

extern const char root_html_start[] asm("_binary_root_html_start");
extern const char root_html_end[] asm("_binary_root_html_end");

static const char *TAG = "wifi_manager";

typedef struct {
    char ssid[33];
    char password[65];
} wifi_mgr_credentials_t;

typedef struct {
    wifi_mgr_mode_t     mode;
    bool                connected;
    int                 retry_count;
    bool                scan_requested;
    bool                reconnect_backoff;
    bool                provisioning;
    EventGroupHandle_t  event_group;
    esp_netif_t        *netif;
    esp_event_handler_instance_t wifi_event_instance;
    esp_event_handler_instance_t ip_event_instance;
    httpd_handle_t      http_server;
    dns_server_handle_t dns_server;
    TaskHandle_t        provisioning_task;
    wifi_mgr_credentials_t sta_credentials;
    wifi_mgr_credentials_t pending_credentials;
    wifi_mgr_callback_t callbacks[WIFI_MGR_MAX_CALLBACKS];
    int                 callback_count;
} wifi_manager_t;

static wifi_manager_t s_mgr = {0};

static void _stop_provisioning_services(void)
{
    if (s_mgr.http_server != NULL) {
        httpd_stop(s_mgr.http_server);
        s_mgr.http_server = NULL;
    }
    if (s_mgr.dns_server != NULL) {
        stop_dns_server(s_mgr.dns_server);
        s_mgr.dns_server = NULL;
    }
}

static void _make_provisioning_ssid(char *ssid, size_t ssid_size)
{
    if (ssid == NULL || ssid_size == 0) {
        return;
    }

    uint8_t mac[6] = {0};
    esp_read_mac(mac, ESP_MAC_WIFI_SOFTAP);
    snprintf(ssid, ssid_size, "ESP32C5%02X%02X%02X%02X%02X%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
}

void wifi_manager_get_provisioning_ssid(char *ssid, size_t ssid_size)
{
    _make_provisioning_ssid(ssid, ssid_size);
}

void wifi_manager_get_sta_ssid(char *ssid, size_t ssid_size)
{
    if (ssid == NULL || ssid_size == 0) {
        return;
    }

    strlcpy(ssid, s_mgr.sta_credentials.ssid, ssid_size);
}

static esp_err_t _parse_wifi_credentials(const char *json, wifi_mgr_credentials_t *credentials)
{
    ESP_RETURN_ON_FALSE(json != NULL && credentials != NULL, ESP_ERR_INVALID_ARG, TAG, "invalid credentials args");

    cJSON *root = cJSON_Parse(json);
    ESP_RETURN_ON_FALSE(root != NULL, ESP_FAIL, TAG, "invalid JSON");

    cJSON *ssid = cJSON_GetObjectItemCaseSensitive(root, "ssid");
    cJSON *password = cJSON_GetObjectItemCaseSensitive(root, "password");
    if (!cJSON_IsString(ssid) || ssid->valuestring == NULL || ssid->valuestring[0] == '\0') {
        cJSON_Delete(root);
        return ESP_FAIL;
    }

    memset(credentials, 0, sizeof(*credentials));
    strlcpy(credentials->ssid, ssid->valuestring, sizeof(credentials->ssid));
    if (cJSON_IsString(password) && password->valuestring != NULL) {
        strlcpy(credentials->password, password->valuestring, sizeof(credentials->password));
    }

    cJSON_Delete(root);
    return ESP_OK;
}

static size_t _copy_wifi_field(uint8_t *dst, size_t dst_size, const char *src)
{
    if (dst == NULL || dst_size == 0 || src == NULL) {
        return 0;
    }

    size_t len = strnlen(src, dst_size);
    memcpy(dst, src, len);
    return len;
}

static void _set_captive_portal_dhcp_url(void)
{
    if (s_mgr.netif == NULL) {
        return;
    }

    esp_netif_ip_info_t ip_info = {0};
    if (esp_netif_get_ip_info(s_mgr.netif, &ip_info) != ESP_OK) {
        return;
    }

    char ip_addr[16] = {0};
    inet_ntoa_r(ip_info.ip.addr, ip_addr, sizeof(ip_addr));

    char captive_uri[32] = {0};
    snprintf(captive_uri, sizeof(captive_uri), "http://%s", ip_addr);

    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_netif_dhcps_stop(s_mgr.netif));
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_netif_dhcps_option(s_mgr.netif, ESP_NETIF_OP_SET,
                                                        ESP_NETIF_CAPTIVEPORTAL_URI,
                                                        captive_uri, strlen(captive_uri)));
    ESP_ERROR_CHECK_WITHOUT_ABORT(esp_netif_dhcps_start(s_mgr.netif));
}

static esp_err_t _root_get_handler(httpd_req_t *req)
{
    const uint32_t root_len = root_html_end - root_html_start;
    httpd_resp_set_type(req, "text/html");
    return httpd_resp_send(req, root_html_start, root_len);
}

static void _provisioning_connect_task(void *arg)
{
    wifi_mgr_credentials_t credentials = s_mgr.pending_credentials;

    vTaskDelay(pdMS_TO_TICKS(350));
    wifi_manager_stop();
    esp_err_t ret = wifi_manager_start_sta(credentials.ssid, credentials.password);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "start STA from provisioning failed: %s", esp_err_to_name(ret));
    }

    s_mgr.provisioning_task = NULL;
    vTaskDelete(NULL);
}

static esp_err_t _wifi_config_post_handler(httpd_req_t *req)
{
    char buf[192] = {0};
    int remaining = req->content_len;
    int offset = 0;

    while (remaining > 0 && offset < (int)sizeof(buf) - 1) {
        int ret = httpd_req_recv(req, buf + offset, MIN(remaining, (int)sizeof(buf) - 1 - offset));
        if (ret <= 0) {
            if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
                continue;
            }
            return ESP_FAIL;
        }
        remaining -= ret;
        offset += ret;
    }
    buf[offset] = '\0';

    wifi_mgr_credentials_t credentials = {0};
    esp_err_t ret = _parse_wifi_credentials(buf, &credentials);
    if (ret != ESP_OK) {
        httpd_resp_set_status(req, "400 Bad Request");
        httpd_resp_sendstr(req, "{\"status\":\"invalid\"}");
        return ESP_OK;
    }

    s_mgr.pending_credentials = credentials;
    s_mgr.sta_credentials = credentials;

    if (s_mgr.provisioning_task == NULL) {
        BaseType_t ok = xTaskCreate(_provisioning_connect_task, "wifi_prov_sta",
                                    WIFI_PROV_TASK_STACK, NULL, 4, &s_mgr.provisioning_task);
        if (ok != pdPASS) {
            httpd_resp_set_status(req, "500 Internal Server Error");
            httpd_resp_sendstr(req, "{\"status\":\"task_failed\"}");
            return ESP_OK;
        }
    }

    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, "{\"status\":\"ok\"}");
    return ESP_OK;
}

static esp_err_t _http_404_error_handler(httpd_req_t *req, httpd_err_code_t err)
{
    (void)err;
    httpd_resp_set_status(req, "302 Temporary Redirect");
    httpd_resp_set_hdr(req, "Location", "/");
    httpd_resp_sendstr(req, "Redirect to captive portal");
    return ESP_OK;
}

static esp_err_t _start_provisioning_services(void)
{
    _set_captive_portal_dhcp_url();

    if (s_mgr.http_server == NULL) {
        httpd_config_t config = HTTPD_DEFAULT_CONFIG();
        config.max_open_sockets = 4;
        config.lru_purge_enable = true;

        ESP_RETURN_ON_ERROR(httpd_start(&s_mgr.http_server, &config), TAG, "start HTTP server failed");

        const httpd_uri_t root = {
            .uri = "/",
            .method = HTTP_GET,
            .handler = _root_get_handler,
            .user_ctx = NULL,
        };
        const httpd_uri_t wifi_config = {
            .uri = "/wifi_config",
            .method = HTTP_POST,
            .handler = _wifi_config_post_handler,
            .user_ctx = NULL,
        };

        ESP_ERROR_CHECK_WITHOUT_ABORT(httpd_register_uri_handler(s_mgr.http_server, &root));
        ESP_ERROR_CHECK_WITHOUT_ABORT(httpd_register_uri_handler(s_mgr.http_server, &wifi_config));
        ESP_ERROR_CHECK_WITHOUT_ABORT(httpd_register_err_handler(s_mgr.http_server, HTTPD_404_NOT_FOUND,
                                                                 _http_404_error_handler));
    }

    if (s_mgr.dns_server == NULL) {
        dns_server_config_t dns_config = DNS_SERVER_CONFIG_SINGLE("*", "WIFI_AP_DEF");
        s_mgr.dns_server = start_dns_server(&dns_config);
    }

    return ESP_OK;
}

static void _delayed_reconnect_task(void *arg)
{
    vTaskDelay(pdMS_TO_TICKS(WIFI_RECONNECT_BACKOFF_MS));

    s_mgr.reconnect_backoff = false;
    if (s_mgr.mode == WIFI_MGR_MODE_STA && !s_mgr.connected && !s_mgr.scan_requested) {
        s_mgr.retry_count = 0;
        xEventGroupClearBits(s_mgr.event_group, WIFI_FAIL_BIT);
        ESP_LOGI(TAG, "Retrying STA connection after backoff");
        esp_err_t ret = esp_wifi_connect();
        if (ret != ESP_OK) {
            ESP_LOGW(TAG, "Delayed STA reconnect failed to start: %s", esp_err_to_name(ret));
        }
    }

    vTaskDelete(NULL);
}

static void _schedule_delayed_reconnect(void)
{
    if (s_mgr.reconnect_backoff || s_mgr.scan_requested) {
        return;
    }

    s_mgr.reconnect_backoff = true;
    BaseType_t ok = xTaskCreate(_delayed_reconnect_task, "wifi_reconn", 3072, NULL, 5, NULL);
    if (ok != pdPASS) {
        s_mgr.reconnect_backoff = false;
        ESP_LOGE(TAG, "Failed to create delayed reconnect task");
    }
}

static void _notify_callbacks(wifi_mgr_event_t event, void *event_data)
{
    for (int i = 0; i < s_mgr.callback_count; i++) {
        if (s_mgr.callbacks[i]) {
            s_mgr.callbacks[i](event, event_data);
        }
    }
}

static void _event_handler(void *arg, esp_event_base_t event_base,
                            int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();

    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        s_mgr.connected = false;
        if (s_mgr.scan_requested) {
            ESP_LOGI(TAG, "STA reconnect paused for user Wi-Fi scan");
            _notify_callbacks(WIFI_MGR_EVENT_DISCONNECTED, event_data);
            return;
        }
        if (s_mgr.retry_count < WIFI_MAX_RETRY) {
            esp_wifi_connect();
            s_mgr.retry_count++;
            ESP_LOGW(TAG, "Retrying... (%d/%d)", s_mgr.retry_count, WIFI_MAX_RETRY);
            _notify_callbacks(WIFI_MGR_EVENT_DISCONNECTED, event_data);
        } else {
            xEventGroupSetBits(s_mgr.event_group, WIFI_FAIL_BIT);
            _notify_callbacks(WIFI_MGR_EVENT_FAIL, event_data);
            _schedule_delayed_reconnect();
        }

    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event->ip_info.ip));
        s_mgr.retry_count = 0;
        s_mgr.connected = true;
        s_mgr.scan_requested = false;
        xEventGroupSetBits(s_mgr.event_group, WIFI_CONNECTED_BIT);
        _notify_callbacks(WIFI_MGR_EVENT_CONNECTED, event_data);

    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED) {
        ESP_LOGI(TAG, "A station connected to SoftAP");
        _notify_callbacks(WIFI_MGR_EVENT_AP_STA_CONNECTED, event_data);

    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        ESP_LOGI(TAG, "A station disconnected from SoftAP");
        _notify_callbacks(WIFI_MGR_EVENT_AP_STA_DISCONNECTED, event_data);
    }
}

esp_err_t wifi_manager_init(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    s_mgr.event_group    = xEventGroupCreate();
    s_mgr.mode           = WIFI_MGR_MODE_NONE;
    s_mgr.connected      = false;
    s_mgr.retry_count    = 0;
    s_mgr.scan_requested = false;
    s_mgr.reconnect_backoff = false;
    s_mgr.provisioning = false;
    s_mgr.http_server = NULL;
    s_mgr.dns_server = NULL;
    s_mgr.provisioning_task = NULL;
    s_mgr.callback_count = 0;
    memset(s_mgr.callbacks, 0, sizeof(s_mgr.callbacks));
    memset(&s_mgr.sta_credentials, 0, sizeof(s_mgr.sta_credentials));
    memset(&s_mgr.pending_credentials, 0, sizeof(s_mgr.pending_credentials));

    ESP_LOGI(TAG, "wifi_manager initialized");
    return ESP_OK;
}

esp_err_t wifi_manager_register_callback(wifi_mgr_callback_t cb)
{
    if (cb == NULL) return ESP_ERR_INVALID_ARG;
    if (s_mgr.callback_count >= WIFI_MGR_MAX_CALLBACKS) {
        ESP_LOGE(TAG, "Callback list full");
        return ESP_ERR_NO_MEM;
    }
    for (int i = 0; i < s_mgr.callback_count; i++) {
        if (s_mgr.callbacks[i] == cb) return ESP_OK;
    }
    s_mgr.callbacks[s_mgr.callback_count++] = cb;
    ESP_LOGI(TAG, "Callback registered (%d total)", s_mgr.callback_count);
    return ESP_OK;
}

esp_err_t wifi_manager_unregister_callback(wifi_mgr_callback_t cb)
{
    for (int i = 0; i < s_mgr.callback_count; i++) {
        if (s_mgr.callbacks[i] == cb) {
            for (int j = i; j < s_mgr.callback_count - 1; j++) {
                s_mgr.callbacks[j] = s_mgr.callbacks[j + 1];
            }
            s_mgr.callbacks[--s_mgr.callback_count] = NULL;
            ESP_LOGI(TAG, "Callback unregistered (%d remaining)", s_mgr.callback_count);
            return ESP_OK;
        }
    }
    ESP_LOGW(TAG, "Callback not found");
    return ESP_ERR_NOT_FOUND;
}

esp_err_t wifi_manager_start_sta(const char *ssid, const char *password)
{
    if (s_mgr.mode != WIFI_MGR_MODE_NONE) {
        ESP_LOGE(TAG, "Wi-Fi already started. Call wifi_manager_stop() first.");
        return ESP_ERR_INVALID_STATE;
    }

    s_mgr.netif = esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
            ESP_EVENT_ANY_ID, &_event_handler, NULL, &s_mgr.wifi_event_instance));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
            IP_EVENT_STA_GOT_IP, &_event_handler, NULL, &s_mgr.ip_event_instance));

    wifi_config_t wifi_config = {
        .sta = {
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .sae_pwe_h2e = WPA3_SAE_PWE_BOTH,
            .sae_h2e_identifier = "",
        },
    };
    _copy_wifi_field(wifi_config.sta.ssid, sizeof(wifi_config.sta.ssid), ssid);
    if (password != NULL) {
        _copy_wifi_field(wifi_config.sta.password, sizeof(wifi_config.sta.password), password);
    }
    strlcpy(s_mgr.sta_credentials.ssid, ssid, sizeof(s_mgr.sta_credentials.ssid));
    strlcpy(s_mgr.sta_credentials.password, password ? password : "", sizeof(s_mgr.sta_credentials.password));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    s_mgr.mode = WIFI_MGR_MODE_STA;
    s_mgr.scan_requested = false;
    s_mgr.reconnect_backoff = false;
    s_mgr.provisioning = false;
    ESP_LOGI(TAG, "STA started, connecting to: %s", ssid);
    return ESP_OK;
}

esp_err_t wifi_manager_start_ap(const char *ssid, const char *password,
                                 uint8_t channel, uint8_t max_conn)
{
    if (s_mgr.mode != WIFI_MGR_MODE_NONE) {
        ESP_LOGE(TAG, "Wi-Fi already started. Call wifi_manager_stop() first.");
        return ESP_ERR_INVALID_STATE;
    }

    s_mgr.netif = esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
            ESP_EVENT_ANY_ID, &_event_handler, NULL, &s_mgr.wifi_event_instance));

    wifi_config_t wifi_config = {
        .ap = {
            .channel        = channel,
            .max_connection = max_conn,
            .authmode       = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg        = { .required = true },
        },
    };
    wifi_config.ap.ssid_len = (uint8_t)_copy_wifi_field(wifi_config.ap.ssid, sizeof(wifi_config.ap.ssid), ssid);
    _copy_wifi_field(wifi_config.ap.password, sizeof(wifi_config.ap.password), password);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    s_mgr.mode = WIFI_MGR_MODE_AP;
    s_mgr.scan_requested = false;
    s_mgr.reconnect_backoff = false;
    s_mgr.provisioning = false;
    ESP_LOGI(TAG, "SoftAP started. SSID:%s channel:%d", ssid, channel);
    return ESP_OK;
}

esp_err_t wifi_manager_start_provisioning(void)
{
    char ap_ssid[33] = {0};
    _make_provisioning_ssid(ap_ssid, sizeof(ap_ssid));

    if (s_mgr.mode != WIFI_MGR_MODE_NONE) {
        ESP_RETURN_ON_ERROR(wifi_manager_stop(), TAG, "stop Wi-Fi before provisioning failed");
    }

    esp_err_t ret = wifi_manager_start_ap(ap_ssid, WIFI_PROV_AP_PASSWORD, 1, 4);
    if (ret != ESP_OK) {
        return ret;
    }

    ret = _start_provisioning_services();
    if (ret != ESP_OK) {
        wifi_manager_stop();
        return ret;
    }

    s_mgr.provisioning = true;
    _notify_callbacks(WIFI_MGR_EVENT_PROVISIONING_STARTED, NULL);
    ESP_LOGI(TAG, "Provisioning AP started: %s", ap_ssid);
    return ESP_OK;
}

esp_err_t wifi_manager_stop_provisioning(bool restart_sta)
{
    if (!s_mgr.provisioning && s_mgr.mode != WIFI_MGR_MODE_AP) {
        return ESP_OK;
    }

    wifi_mgr_credentials_t sta_credentials = s_mgr.sta_credentials;
    esp_err_t ret = wifi_manager_stop();
    if (ret != ESP_OK) {
        return ret;
    }

    if (restart_sta && sta_credentials.ssid[0] != '\0') {
        ret = wifi_manager_start_sta(sta_credentials.ssid, sta_credentials.password);
        if (ret != ESP_OK) {
            ESP_LOGW(TAG, "restart STA after provisioning stop failed: %s", esp_err_to_name(ret));
        }
    }
    return ret;
}

esp_err_t wifi_manager_stop(void)
{
    if (s_mgr.mode == WIFI_MGR_MODE_NONE) return ESP_OK;

    _stop_provisioning_services();

    if (s_mgr.mode == WIFI_MGR_MODE_STA) {
        esp_wifi_disconnect();
    }

    esp_err_t ret = esp_wifi_stop();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "esp_wifi_stop failed: %s", esp_err_to_name(ret));
        return ret;
    }
    if (s_mgr.wifi_event_instance != NULL) {
        ESP_ERROR_CHECK_WITHOUT_ABORT(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID,
                                                                            s_mgr.wifi_event_instance));
        s_mgr.wifi_event_instance = NULL;
    }
    if (s_mgr.ip_event_instance != NULL) {
        ESP_ERROR_CHECK_WITHOUT_ABORT(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP,
                                                                            s_mgr.ip_event_instance));
        s_mgr.ip_event_instance = NULL;
    }
    ESP_ERROR_CHECK(esp_wifi_deinit());

    // 必须在 destroy 之前清理 driver 和 handlers
    ESP_ERROR_CHECK(esp_wifi_clear_default_wifi_driver_and_handlers(s_mgr.netif));
    esp_netif_destroy(s_mgr.netif);
    s_mgr.netif = NULL;

    s_mgr.mode = WIFI_MGR_MODE_NONE;
    s_mgr.connected = false;
    s_mgr.retry_count = 0;
    s_mgr.scan_requested = false;
    s_mgr.reconnect_backoff = false;
    if (s_mgr.provisioning) {
        _notify_callbacks(WIFI_MGR_EVENT_PROVISIONING_STOPPED, NULL);
    }
    s_mgr.provisioning = false;
    xEventGroupClearBits(s_mgr.event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT);

    ESP_LOGI(TAG, "Wi-Fi stopped");
    return ESP_OK;
}

bool wifi_manager_is_connected(void)
{
    return s_mgr.connected;
}

wifi_mgr_mode_t wifi_manager_get_mode(void)
{
    return s_mgr.mode;
}

esp_err_t wifi_manager_prepare_scan(void)
{
    if (s_mgr.mode != WIFI_MGR_MODE_STA || s_mgr.connected) {
        return ESP_OK;
    }

    s_mgr.scan_requested = true;
    esp_err_t ret = esp_wifi_disconnect();
    if (ret == ESP_ERR_WIFI_NOT_STARTED || ret == ESP_ERR_WIFI_NOT_INIT) {
        return ESP_OK;
    }
    if (ret != ESP_OK) {
        s_mgr.scan_requested = false;
    }
    return ret;
}

esp_err_t wifi_manager_finish_scan(void)
{
    if (s_mgr.mode != WIFI_MGR_MODE_STA || s_mgr.connected) {
        s_mgr.scan_requested = false;
        return ESP_OK;
    }

    s_mgr.scan_requested = false;
    s_mgr.reconnect_backoff = false;
    s_mgr.retry_count = 0;
    xEventGroupClearBits(s_mgr.event_group, WIFI_FAIL_BIT);

    esp_err_t ret = esp_wifi_connect();
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "STA reconnect after scan failed to start: %s", esp_err_to_name(ret));
    } else {
        ESP_LOGI(TAG, "STA reconnect resumed after scan");
    }
    return ret;
}
