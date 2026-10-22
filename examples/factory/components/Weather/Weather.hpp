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
     * @brief Weather application for weather data display and update
     *
     */
    class Weather : public systems::phone::App
    {
    public:
        /**
         * @brief Get the singleton instance of Weather
         *
         * @param use_status_bar Show status bar
         * @param use_navigation_bar Show navigation bar
         * @return Weather* Singleton instance pointer
         */
        static Weather *requestInstance(bool use_status_bar = false, bool use_navigation_bar = false);

        /**
         * @brief Destroy the Weather object
         *
         */
        ~Weather();

    protected:
        /**
         * @brief Construct a new Weather object (private to enforce singleton)
         *
         * @param use_status_bar Show status bar
         * @param use_navigation_bar Show navigation bar
         */
        Weather(bool use_status_bar, bool use_navigation_bar);

        bool run(void) override;
        bool back(void) override;
        bool close(void) override;
        bool init(void) override;
        bool deinit(void) override;
        bool pause(void) override;
        bool resume(void) override;

    private:
        static Weather *_instance;
    };

} // namespace esp_brookesia::apps