/* SPDX-License-Identifier: Apache-2.0 */
#pragma once
#include "systems/phone/esp_brookesia_phone_app.hpp"

namespace esp_brookesia::apps {
class DisplayTest : public systems::phone::App {
public:
    static DisplayTest *requestInstance();
    ~DisplayTest() override;
protected:
    bool init() override { return true; }
    bool run() override;
    bool back() override { return notifyCoreClosed(); }
    bool close() override;
    bool deinit() override { return close(); }
    bool pause() override;
    bool resume() override;
private:
    DisplayTest();
    static DisplayTest *instance_;
    static void cycle(lv_timer_t *timer);
    static void touch(lv_event_t *event);
    lv_obj_t *root_ = nullptr;
    lv_timer_t *timer_ = nullptr;
    unsigned color_index_ = 0;
    uint32_t first_tap_tick_ = 0;
    bool waiting_second_tap_ = false;
    bool exiting_ = false;
};
}
