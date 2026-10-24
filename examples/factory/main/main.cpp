/* SPDX-License-Identifier: Apache-2.0 */
#include <new>
#include "bsp/esp-bsp.h"
#include "bsp_audio_session.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "esp_brookesia.hpp"
#include "Settings.hpp"
#include "DrawingBoard.hpp"
#include "Photos.hpp"
#include "Weather.hpp"
#include "Music.hpp"
#include "Sensor.hpp"
#include "Games.hpp"
#include "AIChats.hpp"
#include "MicTest.hpp"
#include "ButtonTest.hpp"
#include "DisplayTest.hpp"
#include "Qmi8658Ball.hpp"
#include "wifi_manager.h"
#include "settings_service.h"
#include "dark/stylesheet.hpp"

using namespace esp_brookesia;
using namespace esp_brookesia::gui;
using namespace esp_brookesia::systems::phone;
using namespace esp_brookesia::apps;
static constexpr char TAG[] = "main";

static bool app_start()
{
    LvLock::registerCallbacks([](int timeout) {
        return bsp_display_lock(timeout > 0 ? uint32_t(timeout) : 0);
    }, []() { bsp_display_unlock(); return true; });
    LvLockGuard guard;
    auto *phone = new(std::nothrow) Phone(bsp_display_get());
    if (!phone) return false;
    if (auto *touch = bsp_display_get_input_dev(); touch && !phone->setTouchDevice(touch)) return false;
    static const Stylesheet style = [] {
        Stylesheet s = STYLESHEET_240_320_DARK;
        s.display.app_launcher.data.icon.label.text_color = StyleColor::COLOR(0xFFFFFF);
        auto &status = s.display.status_bar.data;
        status.main.background_color = StyleColor::COLOR_WITH_OPACITY(0, LV_OPA_TRANSP);
        status.main.text_color = StyleColor::COLOR(0xFFFFFF);
        status.main.text_font = StyleFont::SIZE(24);
        status.area.data[0].layout_column_start_offset = 0;
        status.flags.enable_battery_icon = 0;
        status.flags.enable_wifi_icon = 0;
        s.manager.gesture.flags.enable_indicator_bars[static_cast<int>(Gesture::IndicatorBarType::BOTTOM)] = 0;
        return s;
    }();
    if (!phone->addStylesheet(style) || !phone->activateStylesheet(style) || !phone->begin()) return false;
    struct Entry { const char *name; App *app; };
    const Entry apps[] = {
        {"Display Test", DisplayTest::requestInstance()},
        {"Settings", Settings::requestInstance()},
        {"AIChats", AIChats::requestInstance()},
        {"Touch Test", DrawingBoard::requestInstance()},
        {"Ball", Qmi8658Ball::requestInstance()},
        {"Photos", Photos::requestInstance()},
        {"Weather", Weather::requestInstance()},
        {"Music", Music::requestInstance()},
        {"Sensor", Sensor::requestInstance()},
        {"2048", Games::requestInstance()},
        {"Mic Test", MicTest::requestInstance()},
        {"Button Test", ButtonTest::requestInstance()},
    };
    unsigned installed = 0;
    for (const auto &entry : apps) {
        int id = entry.app ? phone->installApp(entry.app) : -1;
        if (id >= 0) { ++installed; ESP_LOGI(TAG, "%s installed, ID=%d", entry.name, id); }
        else ESP_LOGE(TAG, "%s installation failed", entry.name);
    }
    auto *status = phone->getDisplay().getStatusBar();
    if (status) {
        status->setClockFormat(StatusBar::ClockFormat::FORMAT_24H);
        status->clearClock();
        lv_timer_create([](lv_timer_t *timer) {
            auto *bar = static_cast<StatusBar *>(lv_timer_get_user_data(timer));
            struct tm local = {};
            if (wifi_manager_get_local_time(&local)) bar->setClock(local.tm_hour, local.tm_min);
            else bar->clearClock();
        }, 1000, status);
    }
    ESP_LOGI(TAG, "Desktop ready: 240x320, %u/12 APPs; internal free=%u, largest=%u; PSRAM=%u",
             installed, (unsigned)heap_caps_get_free_size(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT),
             (unsigned)heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT),
             (unsigned)heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    return installed == 12;
}
extern "C" void app_main()
{
    esp_err_t ret = bsp_init();
    if (ret != ESP_OK) { ESP_LOGE(TAG, "Board startup: %s", esp_err_to_name(ret)); return; }
    ret = bsp_extra_codec_init();
    if (ret != ESP_OK) ESP_LOGW(TAG, "Audio session: %s", esp_err_to_name(ret));
    if (!settings_service_start()) ESP_LOGE(TAG, "Settings worker unavailable");
    if (!wifi_manager_init()) ESP_LOGE(TAG, "Network worker unavailable");
    if (!app_start()) ESP_LOGE(TAG, "Some desktop APPs failed; check preceding errors");
    ret = bsp_display_show();
    if (ret != ESP_OK) ESP_LOGE(TAG, "First frame: %s", esp_err_to_name(ret));
}
