/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "Musicplayer.hpp"
#include "lvgl.h"
#include "esp_brookesia.hpp"
#ifdef ESP_UTILS_LOG_TAG
#undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "BS:MusicPlayer"
#include "esp_lib_utils.h"
#include "sdkconfig.h"
#include "bsp/esp-bsp.h"
#include "music_init/music_init.h"
#include "events_init.h"


// 替换为你的音乐APP图标
LV_IMG_DECLARE(img_app_musicplayer);

// 应用名称
#define APP_NAME "MusicPlayer"

namespace esp_brookesia::apps
{

    MusicPlayer *MusicPlayer::_instance = nullptr;

    MusicPlayer *MusicPlayer::requestInstance(bool use_status_bar, bool use_navigation_bar)
    {
        if (_instance == nullptr)
        {
            _instance = new MusicPlayer(use_status_bar, use_navigation_bar);
        }
        return _instance;
    }

    MusicPlayer::MusicPlayer(bool use_status_bar, bool use_navigation_bar) 
        : App(APP_NAME, &img_app_musicplayer, true, use_status_bar, use_navigation_bar)
    {
    }

    MusicPlayer::~MusicPlayer()
    {
    }

    bool MusicPlayer::run(void)
    {
        ESP_UTILS_LOGD("Run");

        init_music_player_ui();

        return true;
    }

    bool MusicPlayer::back(void)
    {
        ESP_UTILS_LOGD("Back");
        // 通知核心关闭当前应用
        ESP_UTILS_CHECK_FALSE_RETURN(notifyCoreClosed(), false, "Notify core closed failed");
        return true;
    }

    bool MusicPlayer::close()
    {
        ESP_UTILS_LOGD("Close - stop music");
        music_close_from_outside();  
        return true;
    }

    bool MusicPlayer::init()
    {
        ESP_UTILS_LOGD("Init");
        return true;
    }

    bool MusicPlayer::deinit()
    {
        ESP_UTILS_LOGD("Deinit");
        music_close_from_outside();
        return true;
    }

    bool MusicPlayer::pause()
    {
        ESP_UTILS_LOGD("Pause - release audio for other APPs");
        music_close_from_outside();
        return true;
    }

    bool MusicPlayer::resume()
    {
        ESP_UTILS_LOGD("Resume");
        init_music_player_ui();
        return true;
    }
} // namespace esp_brookesia::apps
