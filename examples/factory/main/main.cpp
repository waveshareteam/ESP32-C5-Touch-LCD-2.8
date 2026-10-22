#include "esp_log.h"
#include "bsp/esp-bsp.h"
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_sntp.h"
#include "iot_button.h"
#include "button_gpio.h"
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>

#include "esp_brookesia.hpp"
#include "boost/thread.hpp"
#ifdef ESP_UTILS_LOG_TAG
#   undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "Main"
#include "esp_lib_utils.h"

#include "AIChats.hpp"
#include "MicTest.hpp"
#include "DrawingBoard.hpp"
#include "Games.hpp"
#include "Qmi8658Ball.hpp"
#include "Musicplayer.hpp"



#include "bsp_board_extra.h"
#include "Settings.hpp"
#include "Photos.hpp"
#include "Weather.hpp"
#include "Sensor.hpp"
#include "system_manage_service.h"


#include "./dark/stylesheet.hpp"

using namespace esp_brookesia;
using namespace esp_brookesia::gui;
using namespace esp_brookesia::systems::phone;
using namespace esp_brookesia::apps;

static const char *TAG = "main";

constexpr bool EXAMPLE_SHOW_MEM_INFO = true;
static bool s_sntp_started = false;
static sys_manage_service_t *s_system_service = nullptr;

static void set_default_beijing_time(void)
{
    struct timeval tv = {};
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    settimeofday(&tv, nullptr);
}

static StatusBar::WifiState wifi_rssi_to_status_bar_state(int8_t rssi)
{
    if (rssi >= -60) {
        return StatusBar::WifiState::SIGNAL_3;
    }
    if (rssi >= -75) {
        return StatusBar::WifiState::SIGNAL_2;
    }
    return StatusBar::WifiState::SIGNAL_1;
}

static void start_sntp_once(void)
{
    if (s_sntp_started || esp_sntp_enabled()) {
        s_sntp_started = true;
        return;
    }

    esp_sntp_setoperatingmode(ESP_SNTP_OPMODE_POLL);
    esp_sntp_setservername(0, "ntp.aliyun.com");
    esp_sntp_setservername(1, "pool.ntp.org");
    esp_sntp_set_time_sync_notification_cb([](struct timeval *tv) {
        struct tm timeinfo;
        localtime_r(&tv->tv_sec, &timeinfo);
        ESP_LOGI(TAG, "SNTP time synced: %02d:%02d:%02d",
                 timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    });
    esp_sntp_init();
    s_sntp_started = true;
    ESP_LOGI(TAG, "SNTP time sync started");
}

extern "C" void app_main(void)
{

    setenv("TZ", "CST-8", 1);
    tzset();
    set_default_beijing_time();

    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    s_system_service = system_manage_service_init();

    bsp_io_expander_init();
    esp_err_t audio_ret = bsp_extra_codec_init();
    if (audio_ret != ESP_OK) {
        ESP_LOGW(TAG, "Audio unavailable: %s", esp_err_to_name(audio_ret));
    }
    vTaskDelay(pdMS_TO_TICKS(100));
    bsp_spiffs_mount();
    
    esp_err_t sd_ret = bsp_sdcard_mount();
    if (sd_ret != ESP_OK) {
        ESP_UTILS_LOGW("SD card mount failed: %s", esp_err_to_name(sd_ret));
    }

    ESP_UTILS_LOGI("Display ESP-Brookesia phone demo");
    ESP_UTILS_CHECK_NULL_EXIT(bsp_display_start(), "Start display failed");
    ESP_UTILS_CHECK_ERROR_EXIT(bsp_display_backlight_on(), "Turn on display backlight failed");

    /* Configure GUI lock */
    LvLock::registerCallbacks([](int timeout_ms) {
        esp_err_t ret = bsp_display_lock(timeout_ms);
        ESP_UTILS_CHECK_FALSE_RETURN(ret == ESP_OK, false, "Lock failed (timeout_ms: %d)", timeout_ms);
        return true;
    }, []() {
        bsp_display_unlock();
        return true;
    });


    /* Create a phone object */
    Phone *phone = new (std::nothrow) Phone();
    ESP_UTILS_CHECK_NULL_EXIT(phone, "Create phone failed");

    // ===================== 核心：加载并激活 240×320 深色主题 =====================
    Stylesheet *stylesheet = new systems::phone::Stylesheet(STYLESHEET_240_320_DARK);
    ESP_UTILS_CHECK_NULL_EXIT(stylesheet, "Create stylesheet failed");
    ESP_UTILS_CHECK_FALSE_EXIT(phone->addStylesheet(stylesheet), "Add stylesheet failed");
    ESP_UTILS_CHECK_FALSE_EXIT(phone->activateStylesheet(stylesheet), "Activate stylesheet failed");
    delete stylesheet; 
    // ============================================================================

    {
        // 在非GUI任务中操作LVGL前需要获取锁
        LvLockGuard gui_guard;

        /* Begin the phone */
        ESP_UTILS_CHECK_FALSE_EXIT(phone->begin(), "Begin failed");

        /* 初始化并安装系统注册表中已有的应用（如设置、文件管理器等） */
        std::vector<systems::base::Manager::RegistryAppInfo> inited_apps;
        ESP_UTILS_CHECK_FALSE_EXIT(phone->initAppFromRegistry(inited_apps), "Init app registry failed");
        ESP_UTILS_CHECK_FALSE_EXIT(phone->installAppFromRegistry(inited_apps), "Install app registry failed");


        auto app_settings = esp_brookesia::apps::Settings::requestInstance();
        ESP_UTILS_CHECK_FALSE_EXIT(phone->installApp(app_settings),  "Install Settings failed");

        auto app_AIChats = esp_brookesia::apps::AIChats::requestInstance();
        ESP_UTILS_CHECK_FALSE_EXIT(phone->installApp(app_AIChats),"Install AIChats failed");

        auto app_Sensor = esp_brookesia::apps::Sensor::requestInstance();
        ESP_UTILS_CHECK_FALSE_EXIT(phone->installApp(app_Sensor), "Install Sensor failed");

        auto app_DrawingBoard = esp_brookesia::apps::DrawingBoard::requestInstance();
        ESP_UTILS_CHECK_FALSE_EXIT(phone->installApp(app_DrawingBoard), "Install Drawpanel failed");

        auto app_Qmiball = esp_brookesia::apps::Qmi8658Ball::requestInstance();
        ESP_UTILS_CHECK_FALSE_EXIT(phone->installApp(app_Qmiball), "Install Qmiball failed");

        auto app_Musicplayer = esp_brookesia::apps::MusicPlayer::requestInstance();
        ESP_UTILS_CHECK_FALSE_EXIT(phone->installApp(app_Musicplayer), "Install MusicPlayer failed");

        auto app_Weather = esp_brookesia::apps::Weather::requestInstance();
        ESP_UTILS_CHECK_FALSE_EXIT(phone->installApp(app_Weather), "Install Weather failed");

        auto app_Games = esp_brookesia::apps::Games::requestInstance();
        ESP_UTILS_CHECK_FALSE_EXIT(phone->installApp(app_Games), "Install Games failed");

        auto app_Photos = esp_brookesia::apps::Photos::requestInstance();
        ESP_UTILS_CHECK_FALSE_EXIT(phone->installApp(app_Photos),"Install Photos failed");

        auto app_mic = esp_brookesia::apps::MicTest::requestInstance();
        ESP_UTILS_CHECK_FALSE_EXIT(phone->installApp(app_mic), "Install Mic Test failed");

        ESP_LOGI(TAG, "Desktop ready: internal free=%u, largest=%u; DMA free=%u, largest=%u; PSRAM free=%u",
                 (unsigned)heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT),
                 (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT),
                 (unsigned)heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA),
                 (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL | MALLOC_CAP_DMA),
                 (unsigned)heap_caps_get_free_size(MALLOC_CAP_SPIRAM));

        /* 创建定时器，每秒更新状态栏时钟 */
        lv_timer_create([](lv_timer_t *t) {
            time_t now;
            struct tm timeinfo;
            Phone *phone = (Phone *)t->user_data;

            ESP_UTILS_CHECK_NULL_EXIT(phone, "Invalid phone");

            time(&now);
            localtime_r(&now, &timeinfo);

            ESP_UTILS_CHECK_FALSE_EXIT(
                phone->getDisplay().getStatusBar()->setClock(timeinfo.tm_hour, timeinfo.tm_min),
                "Refresh status bar failed"
            );

            wifi_ap_record_t ap_info = {};
            StatusBar::WifiState wifi_state = StatusBar::WifiState::DISCONNECTED;
            if (s_system_service != nullptr && s_system_service->wifi_connected &&
                esp_wifi_sta_get_ap_info(&ap_info) == ESP_OK) {
                wifi_state = wifi_rssi_to_status_bar_state(ap_info.rssi);
                start_sntp_once();
            }
            ESP_UTILS_CHECK_FALSE_EXIT(
                phone->getDisplay().getStatusBar()->setWifiIconState(wifi_state),
                "Refresh status bar WiFi failed"
            );
        }, 1000, phone);
    }


    if constexpr (EXAMPLE_SHOW_MEM_INFO) {
        esp_utils::thread_config_guard thread_config({
            .name = "mem_info",
            .stack_size = 4096,
        });
        boost::thread([ = ]() {
            char buffer[128];
            size_t internal_free = 0;
            size_t internal_total = 0;
            size_t external_free = 0;
            size_t external_total = 0;

            while (1) {
                internal_free = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
                internal_total = heap_caps_get_total_size(MALLOC_CAP_INTERNAL);
                external_free = heap_caps_get_free_size(MALLOC_CAP_SPIRAM);
                external_total = heap_caps_get_total_size(MALLOC_CAP_SPIRAM);
                sprintf(buffer,
                        "\t           Biggest /     Free /    Total\n"
                        "\t  SRAM : [%8d / %8d / %8d]\n"
                        "\t PSRAM : [%8d / %8d / %8d]",
                        heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL), internal_free, internal_total,
                        heap_caps_get_largest_free_block(MALLOC_CAP_SPIRAM), external_free, external_total);

                {
                    LvLockGuard gui_guard;
                    ESP_UTILS_CHECK_FALSE_EXIT(
                        phone->getDisplay().getRecentsScreen()->setMemoryLabel(
                            internal_free / 1024, internal_total / 1024, external_free / 1024, external_total / 1024
                        ), "Set memory label failed"
                    );
                }

                boost::this_thread::sleep_for(boost::chrono::seconds(5));
            }
        }).detach();
    }
}
