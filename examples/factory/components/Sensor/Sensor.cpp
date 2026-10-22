/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "Sensor.hpp"
#include "lvgl.h"
#include "esp_brookesia.hpp"
#ifdef ESP_UTILS_LOG_TAG
#undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "BS:Sensor"
#include "esp_lib_utils.h"
#include "sdkconfig.h"
#include "bsp/esp-bsp.h"
#include "sensor_init/sensor_init.h"

LV_IMG_DECLARE(img_app_sensor);

#define APP_NAME "Sensor"

namespace esp_brookesia::apps
{

Sensor *Sensor::_instance = nullptr;

Sensor *Sensor::requestInstance(bool use_status_bar, bool use_navigation_bar)
{
    if (_instance == nullptr) {
        _instance = new Sensor(use_status_bar, use_navigation_bar);
    }
    return _instance;
}

Sensor::Sensor(bool use_status_bar, bool use_navigation_bar)
    : App(APP_NAME, &img_app_sensor, true, use_status_bar, use_navigation_bar)
{
}

Sensor::~Sensor()
{
}

bool Sensor::run(void)
{
    ESP_UTILS_LOGD("Run");
    init_sensor_ui();
    return true;
}

bool Sensor::back(void)
{
    ESP_UTILS_LOGD("Back");
    deinit_sensor_ui();
    ESP_UTILS_CHECK_FALSE_RETURN(notifyCoreClosed(), false, "Notify core closed failed");
    return true;
}

bool Sensor::close(void)
{
    ESP_UTILS_LOGD("Close");
    deinit_sensor_ui();
    return true;
}

bool Sensor::init(void)
{
    ESP_UTILS_LOGD("Init");
    return true;
}

bool Sensor::deinit(void)
{
    ESP_UTILS_LOGD("Deinit");
    deinit_sensor_ui();
    return true;
}

bool Sensor::pause(void)
{
    ESP_UTILS_LOGD("Pause");
    return true;
}

bool Sensor::resume(void)
{
    ESP_UTILS_LOGD("Resume");
    return true;
}

} // namespace esp_brookesia::apps
