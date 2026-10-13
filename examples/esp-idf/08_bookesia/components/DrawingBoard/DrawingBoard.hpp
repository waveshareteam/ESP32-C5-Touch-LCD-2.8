#pragma once

#include "systems/phone/esp_brookesia_phone_app.hpp"
#include "lvgl.h"

namespace esp_brookesia::apps
{
    class DrawingBoard : public systems::phone::App
    {
    public:
        static DrawingBoard *requestInstance(bool use_status_bar = false, bool use_navigation_bar = false);
        ~DrawingBoard();

    protected:
        DrawingBoard(bool use_status_bar, bool use_navigation_bar);

        bool run(void) override;
        bool back(void) override;
        bool close(void) override;
        bool init(void) override;
        bool deinit(void) override;
        bool pause(void) override;
        bool resume(void) override;

    private:
        static DrawingBoard *_instance;
        static void _touch_event_cb(lv_event_t *e);
    };
}