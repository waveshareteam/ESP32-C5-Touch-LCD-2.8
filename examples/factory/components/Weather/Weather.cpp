/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "Weather.hpp"
#include "lvgl.h"
#include "esp_brookesia.hpp"
#ifdef ESP_UTILS_LOG_TAG
#undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "BS:Weather"
#include "esp_lib_utils.h"
#include "sdkconfig.h"
#include "bsp/esp-bsp.h"
#include "weather_init/weather_init.h"

// 2. 替换为你的天气APP图标
LV_IMG_DECLARE(img_app_weather);

// 3. 修改应用名称为天气
#define APP_NAME "Weather"

namespace esp_brookesia::apps
{
    // 4. 类名全部改为 Weather
    Weather *Weather::_instance = nullptr;

    Weather *Weather::requestInstance(bool use_status_bar, bool use_navigation_bar)
    {
        if (_instance == nullptr)
        {
            _instance = new Weather(use_status_bar, use_navigation_bar);
        }
        return _instance;
    }

    Weather::Weather(bool use_status_bar, bool use_navigation_bar) 
        : App(APP_NAME, &img_app_weather, true, use_status_bar, use_navigation_bar)
    {
    }

    Weather::~Weather()
    {
    }

    bool Weather::run(void)
    {
        ESP_UTILS_LOGD("Run");

        init_weather_ui();

        return true;
    }

    bool Weather::back(void)
    {
        ESP_UTILS_LOGD("Back");
        // 通知核心关闭当前应用
        ESP_UTILS_CHECK_FALSE_RETURN(notifyCoreClosed(), false, "Notify core closed failed");
        return true;
    }

    bool Weather::close(void)
    {
        ESP_UTILS_LOGD("Close");
        deinit_weather_ui();
        return true;
    }

    bool Weather::init()
    {
        ESP_UTILS_LOGD("Init");
        return true;
    }

    bool Weather::deinit()
    {
        ESP_UTILS_LOGD("Deinit");
        return true;
    }

    bool Weather::pause()
    {
        ESP_UTILS_LOGD("Pause");
        // 天气APP无需暂停逻辑，保留空实现
        return true;
    }

    bool Weather::resume()
    {
        ESP_UTILS_LOGD("Resume");
        // 天气APP无需恢复逻辑，保留空实现
        return true;
    }

} // namespace esp_brookesia::apps
