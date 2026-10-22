/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once

#include "systems/phone/esp_brookesia_phone_app.hpp"
#include "lvgl.h"

namespace esp_brookesia::apps
{

    /**
     * @brief MusicPlayer application for music playback and control
     *
     */
    class MusicPlayer : public systems::phone::App
    {
    public:
        /**
         * @brief Get the singleton instance of MusicPlayer
         *
         * @param use_status_bar Show status bar
         * @param use_navigation_bar Show navigation bar
         * @return MusicPlayer* Singleton instance pointer
         */
        static MusicPlayer *requestInstance(bool use_status_bar = false, bool use_navigation_bar = false);

        /**
         * @brief Destroy the MusicPlayer object
         *
         */
        ~MusicPlayer();

    protected:
        /**
         * @brief Construct a new MusicPlayer object (private to enforce singleton)
         *
         * @param use_status_bar Show status bar
         * @param use_navigation_bar Show navigation bar
         */
        MusicPlayer(bool use_status_bar, bool use_navigation_bar);

        bool run(void) override;
        bool back(void) override;
        bool close(void) override;
        bool init(void) override;
        bool deinit(void) override;
        bool pause(void) override;
        bool resume(void) override;

    private:
        static MusicPlayer *_instance;
    };

} // namespace esp_brookesia::apps