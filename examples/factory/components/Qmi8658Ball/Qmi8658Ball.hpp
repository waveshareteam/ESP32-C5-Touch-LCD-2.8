/* SPDX-License-Identifier: Apache-2.0 */
#pragma once
#include "systems/phone/esp_brookesia_phone_app.hpp"
#include "app_support.hpp"
namespace esp_brookesia::apps {
class Qmi8658Ball : public systems::phone::App, private AppWorker {
public:
    static Qmi8658Ball *requestInstance();
    ~Qmi8658Ball() override;
protected:
    bool init() override { return true; }
    bool run() override;
    bool back() override { return notifyCoreClosed(); }
    bool close() override;
    bool deinit() override { return close(); }
    bool pause() override;
    bool resume() override;
private:
    Qmi8658Ball();
    static Qmi8658Ball *instance_;
    static void event(lv_event_t *);
    static void poll(lv_timer_t *);
    void work() override;
    void refresh();
    static constexpr int BALL_SIZE = 36;
    static constexpr int MAX_X = BSP_LCD_H_RES - BALL_SIZE;
    static constexpr int MAX_Y = BSP_LCD_V_RES - BALL_SIZE;
    struct State { float x=MAX_X/2.0f,y=MAX_Y/2.0f; unsigned progress=0; bool calibrated=false; esp_err_t error=ESP_OK; };
    State state_;
    portMUX_TYPE lock_=portMUX_INITIALIZER_UNLOCKED;
    lv_obj_t *root_=nullptr,*ball_=nullptr,*hint_=nullptr,*progress_=nullptr;
    lv_timer_t *timer_=nullptr;
};
}
