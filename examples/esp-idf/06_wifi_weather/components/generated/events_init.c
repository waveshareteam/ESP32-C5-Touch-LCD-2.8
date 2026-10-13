/********************************************************************************************************
 * 固定城市：深圳
 * 模块化：WiFi + 天气API + 解析 + 中英翻译 + UI自动刷新
 * 客户二次开发：仅需修改 城市名/API/翻译表 即可
 ********************************************************************************************************/
#include "events_init.h"
#include "lvgl.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "cJSON.h"
#include "esp_log.h"
#include <netdb.h> 
#include "bsp/esp-bsp.h"
#include "bsp/display.h"
#include "freertos/event_groups.h"

#define WIFI_SSID          "weather-wifi"
#define WIFI_PASSWORD      "12345678"
#define WEB_SERVER         "cn.apihz.cn"
#define WEB_PORT           "80"

// 固定城市：深圳（客户二次开发仅修改这里）
#define WEB_PATH           "/api/tianqi/tqyb.php?id=10015813&key=070d8d7113bef63bbad7216a89cb00a4&sheng=广东&place=深圳"

static const char *TAG = "weather";
static const char *HTTP_REQ = "GET " WEB_PATH " HTTP/1.0\r\nHost: "WEB_SERVER":"WEB_PORT"\r\nUser-Agent: esp-idf/1.0 esp32\r\n\r\n";

// ===================== 全局UI数据 =====================
typedef struct {
    char city[16];       // 城市英文
    char date[16];       // 日期
    char weather[16];    // 天气英文
    char temp[16];       // 温度
    char humi[16];       // 湿度
    char wind_speed[16]; // 风速
    char wind_dir[4];    // 风向缩写
    bool data_ready;     // 数据就绪标志
} weather_data_t;

static weather_data_t s_weather = {0};
static lv_ui *s_ui = NULL;

// 天气中文→英文
static const char* weather_2_en(const char *cn) {
    if (strcmp(cn, "晴")         == 0) return "Sunny";
    if (strcmp(cn, "多云")       == 0) return "Cloudy";
    if (strcmp(cn, "阴")         == 0) return "Overcast";
    if (strcmp(cn, "小雨")       == 0) return "Light Rain";
    if (strcmp(cn, "中雨")       == 0) return "Moderate Rain";
    if (strcmp(cn, "大雨")       == 0) return "Heavy Rain";
    if (strcmp(cn, "雷阵雨")     == 0) return "Thunder Shower";
    if (strcmp(cn, "雾")         == 0) return "Fog";
    if (strcmp(cn, "霾")         == 0) return "Haze";
    if (strcmp(cn, "阵雨")       == 0) return "Shower";
    return "Unknown";
}
// 风向中文
static const char* wind_2_en(const char *cn) {
    if (strcmp(cn, "东风")   == 0) return "E";
    if (strcmp(cn, "南风")   == 0) return "S";
    if (strcmp(cn, "西风")   == 0) return "W";
    if (strcmp(cn, "北风")   == 0) return "N";
    if (strcmp(cn, "东南风") == 0) return "SE";
    if (strcmp(cn, "东北风") == 0) return "NE";
    if (strcmp(cn, "西南风") == 0) return "SW";
    if (strcmp(cn, "西北风") == 0) return "NW";
    return "--";
}

// UI刷新
static void ui_timer(lv_timer_t *timer) {
    if (!s_ui || !s_weather.data_ready) return;

    lv_label_set_text(s_ui->screen_diqu_lab,    s_weather.city);
    lv_label_set_text(s_ui->screen_data_lab,    s_weather.date);
    lv_label_set_text(s_ui->screen_weather_lab, s_weather.weather);
    lv_label_set_text(s_ui->screen_wendu_lab,   s_weather.temp);
    lv_label_set_text(s_ui->screen_shidu_lab,   s_weather.humi);
    lv_label_set_text(s_ui->screen_fengsu_lab,  s_weather.wind_speed);
    lv_label_set_text(s_ui->screen_fengxiang_lab, s_weather.wind_dir);

    s_weather.data_ready = false;
}

// 解析天气
static void parse_weather(const char *data) {
    char *json = strstr(data, "{");
    if (!json) return;

    cJSON *root = cJSON_Parse(json);
    if (!root) { cJSON_Delete(root); return; }

    memset(&s_weather, 0, sizeof(s_weather));

    // 固定城市：深圳
    strcpy(s_weather.city, "Shenzhen");

    // 解析基础数据
    cJSON *weather = cJSON_GetObjectItemCaseSensitive(root, "weather1");
    cJSON *uptime  = cJSON_GetObjectItemCaseSensitive(root, "uptime");
    cJSON *nowinfo = cJSON_GetObjectItemCaseSensitive(root, "nowinfo");

    // 天气+日期
    if (cJSON_IsString(weather)) strcpy(s_weather.weather, weather_2_en(weather->valuestring));
    if (cJSON_IsString(uptime))  strncpy(s_weather.date, uptime->valuestring, 10);

    // 解析实时数据 
    if (nowinfo) {
        cJSON *temp = cJSON_GetObjectItemCaseSensitive(nowinfo, "temperature");
        cJSON *hum  = cJSON_GetObjectItemCaseSensitive(nowinfo, "humidity");
        cJSON *wdir = cJSON_GetObjectItemCaseSensitive(nowinfo, "windDirection");
        cJSON *wspd = cJSON_GetObjectItemCaseSensitive(nowinfo, "windSpeed");

        if (cJSON_IsNumber(temp)) sprintf(s_weather.temp, "%.1f", temp->valuedouble);
        if (cJSON_IsNumber(hum))  sprintf(s_weather.humi, "%.0f", hum->valuedouble);
        if (cJSON_IsString(wdir)) strcpy(s_weather.wind_dir, wind_2_en(wdir->valuestring));
        if (cJSON_IsNumber(wspd)) sprintf(s_weather.wind_speed, "%.1f", wspd->valuedouble);
    }

    ESP_LOGI(TAG, "=====================================");
    ESP_LOGI(TAG, "城市：%s | 日期：%s | 天气：%s", s_weather.city, s_weather.date, s_weather.weather);
    ESP_LOGI(TAG, "温度：%s | 湿度：%s", s_weather.temp, s_weather.humi);
    ESP_LOGI(TAG, "风速：%s | 风向：%s", s_weather.wind_speed, s_weather.wind_dir);
    ESP_LOGI(TAG, "=====================================");

    s_weather.data_ready = true;
    cJSON_Delete(root);
    ESP_LOGI(TAG, "数据解析完成，UI已刷新");
}

// 天气任务
static void weather_task(void *pv) {
    char recv_buf[1024];
    char *data_buf = malloc(2048);
    if (!data_buf) { vTaskDelete(NULL); return; }

    vTaskDelay(pdMS_TO_TICKS(3000));
    static bool first_run = true;

    while (1) {
        memset(data_buf, 0, 2048);
        struct addrinfo hints = {.ai_family = AF_INET, .ai_socktype = SOCK_STREAM}, *res;
        int sock, ret;

        // DNS解析
        if (getaddrinfo(WEB_SERVER, WEB_PORT, &hints, &res) != 0) {
            // 无网络3秒重试（核心修改，解决后开热点慢）
            vTaskDelay(pdMS_TO_TICKS(3000));
            continue;
        }
        // 创建socket
        sock = socket(res->ai_family, res->ai_socktype, 0);
        if (sock < 0) { freeaddrinfo(res); vTaskDelay(pdMS_TO_TICKS(3000)); continue; }
        // 连接服务器
        if (connect(sock, res->ai_addr, res->ai_addrlen) != 0) { close(sock); freeaddrinfo(res); vTaskDelay(pdMS_TO_TICKS(3000)); continue; }
        freeaddrinfo(res);

        // 发送请求
        write(sock, HTTP_REQ, strlen(HTTP_REQ));
        // 超时设置
        struct timeval tv = {5, 0};
        setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

        // 接收数据
        int len = 0;
        do {
            bzero(recv_buf, sizeof(recv_buf));
            ret = read(sock, recv_buf, sizeof(recv_buf)-1);
            if (ret > 0 && len + ret < 2048) {
                memcpy(data_buf + len, recv_buf, ret);
                len += ret;
            }
        } while (ret > 0);

        // 解析数据
        parse_weather(data_buf);
        close(sock);

        // 成功后5分钟刷新
        if(!first_run){
            vTaskDelay(pdMS_TO_TICKS(300000)); 
        }
        first_run = false;
    }

    free(data_buf);
    vTaskDelete(NULL);
}

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        // 热点未开启或断线时，持续重试
        ESP_LOGW(TAG, "WiFi断开，正在重试连接...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
        ESP_LOGI(TAG, "获取到IP: " IPSTR, IP2STR(&event->ip_info.ip));
    }
}

static void wifi_init(void) {
    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // 注册事件处理函数
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL, NULL));

    wifi_config_t wifi_cfg = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD,
        }
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_ERROR_CHECK(esp_wifi_set_band_mode(WIFI_BAND_MODE_AUTO));
    ESP_LOGI(TAG, "WiFi初始化完成，等待连接...");
}

void events_init(lv_ui *ui) {
    s_ui = ui;
    lv_timer_create(ui_timer, 500, NULL);

    wifi_init();
    xTaskCreate(weather_task, "weather_task", 4096, NULL, 5, NULL);
}