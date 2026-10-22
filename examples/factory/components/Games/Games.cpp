/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "Games.hpp"
#include "lvgl.h"
#include "esp_brookesia.hpp"
#ifdef ESP_UTILS_LOG_TAG
#undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "BS:Games"
#include "esp_lib_utils.h"
#include "sdkconfig.h"
#include "bsp/esp-bsp.h"
#include "game_init/game_init.h"

LV_IMG_DECLARE(img_app_2048);


#define APP_NAME "Games"

namespace esp_brookesia::apps
{

    Games *Games::_instance = nullptr;

    Games *Games::requestInstance(bool use_status_bar, bool use_navigation_bar)
    {
        if (_instance == nullptr)
        {
            _instance = new Games(use_status_bar, use_navigation_bar);
        }
        return _instance;
    }

    Games::Games(bool use_status_bar, bool use_navigation_bar) : App(APP_NAME, &img_app_2048, true, use_status_bar, use_navigation_bar)
    {
    }

    Games::~Games()
    {
    }

    bool Games::run(void)
    {
        ESP_UTILS_LOGD("Run");

        // 初始化游戏选择界面，例如显示游戏列表
        init_game_ui_screen();

        return true;
    }

    bool Games::back(void)
    {
        ESP_UTILS_LOGD("Back");
        // 通知核心关闭当前应用
        ESP_UTILS_CHECK_FALSE_RETURN(notifyCoreClosed(), false, "Notify core closed failed");
        return true;
    }

    bool Games::close(void)
    {
        ESP_UTILS_LOGD("Close");
        // 清理游戏资源（如果有）
        return true;
    }

    bool Games::init()
    {
        ESP_UTILS_LOGD("Init");
        // 预加载游戏资源等
        return true;
    }

    bool Games::deinit()
    {
        ESP_UTILS_LOGD("Deinit");
        return true;
    }

    bool Games::pause()
    {
        ESP_UTILS_LOGD("Pause");
        // 暂停正在运行的游戏（如果有）
        return true;
    }

    bool Games::resume()
    {
        ESP_UTILS_LOGD("Resume");
        // 恢复游戏状态
        return true;
    }

} // namespace esp_brookesia::apps