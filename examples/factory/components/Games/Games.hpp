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
     * @brief Games application for displaying and launching games
     *
     */
    class Games : public systems::phone::App
    {
    public:
        /**
         * @brief Get the singleton instance of Games
         *
         * @param use_status_bar Show status bar
         * @param use_navigation_bar Show navigation bar
         * @return Games* Singleton instance pointer
         */
        static Games *requestInstance(bool use_status_bar = false, bool use_navigation_bar = false);

        /**
         * @brief Destroy the Games object
         *
         */
        ~Games();

    protected:
        /**
         * @brief Construct a new Games object (private to enforce singleton)
         *
         * @param use_status_bar Show status bar
         * @param use_navigation_bar Show navigation bar
         */
        Games(bool use_status_bar, bool use_navigation_bar);

        bool run(void) override;
        bool back(void) override;
        bool close(void) override;
        bool init(void) override;
        bool deinit(void) override;
        bool pause(void) override;
        bool resume(void) override;

    private:
        static Games *_instance;
    };

} // namespace esp_brookesia::apps