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

class Sensor : public systems::phone::App
{
public:
    static Sensor *requestInstance(bool use_status_bar = false, bool use_navigation_bar = false);

    ~Sensor();

protected:
    Sensor(bool use_status_bar, bool use_navigation_bar);

    bool run(void) override;
    bool back(void) override;
    bool close(void) override;
    bool init(void) override;
    bool deinit(void) override;
    bool pause(void) override;
    bool resume(void) override;

private:
    static Sensor *_instance;
};

} // namespace esp_brookesia::apps
