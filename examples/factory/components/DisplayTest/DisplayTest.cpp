/* SPDX-License-Identifier: Apache-2.0 */
#include "DisplayTest.hpp"
#include "esp_log.h"
#include <new>

LV_IMAGE_DECLARE(img_app_display_test);
namespace {
constexpr uint32_t COLORS[] = {0xff0000, 0x00ff00, 0x0000ff, 0xffff00, 0xffffff};
constexpr uint32_t DOUBLE_TAP_MS = 400;
esp_brookesia::systems::phone::App::Config fullscreen_config()
{
    auto config = esp_brookesia::systems::phone::App::Config::SIMPLE_CONSTRUCTOR(
        &img_app_display_test, false, false);
    config.flags.enable_navigation_gesture = false;
    return config;
}
}
namespace esp_brookesia::apps {
DisplayTest *DisplayTest::instance_;
DisplayTest *DisplayTest::requestInstance()
{
    if (!instance_) instance_ = new (std::nothrow) DisplayTest;
    return instance_;
}
DisplayTest::DisplayTest()
    : App(systems::base::App::Config::SIMPLE_CONSTRUCTOR("Display Test", &img_app_display_test, true),
          fullscreen_config()) {}
DisplayTest::~DisplayTest() { close(); instance_ = nullptr; }
bool DisplayTest::run()
{
    const auto area = getVisualArea();
    color_index_ = 0;
    waiting_second_tap_ = false;
    exiting_ = false;
    root_ = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(root_);
    lv_obj_set_pos(root_, area.x1, area.y1);
    lv_obj_set_size(root_, lv_area_get_width(&area), lv_area_get_height(&area));
    lv_obj_remove_flag(root_, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_CHAIN | LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_flag(root_, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_PRESS_LOCK);
    lv_obj_set_style_bg_opa(root_, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(root_, lv_color_hex(COLORS[0]), 0);
    lv_obj_add_event_cb(root_, touch, LV_EVENT_ALL, this);
    timer_ = lv_timer_create(cycle, 1000, this);
    if (!timer_) { close(); return false; }
    ESP_LOGI("display_test", "Red -> green -> blue -> yellow -> white, 1 s each; LVGL double tap anywhere to exit");
    return true;
}
void DisplayTest::cycle(lv_timer_t *timer)
{
    auto *app = static_cast<DisplayTest *>(lv_timer_get_user_data(timer));
    app->color_index_ = (app->color_index_ + 1) % (sizeof(COLORS) / sizeof(COLORS[0]));
    // Keep the same input object across color changes so a tap is never lost.
    lv_obj_set_style_bg_color(app->root_, lv_color_hex(COLORS[app->color_index_]), 0);
}
void DisplayTest::touch(lv_event_t *event)
{
    auto *app = static_cast<DisplayTest *>(lv_event_get_user_data(event));
    if (app->exiting_) return;
    const auto code = lv_event_get_code(event);
    if (code == LV_EVENT_LONG_PRESSED || code == LV_EVENT_PRESS_LOST) {
        app->waiting_second_tap_ = false;
    } else if (code == LV_EVENT_SHORT_CLICKED) {
        // LVGL provides clicks; keep the pair local to this APP, independent of
        // the input device's global click streak and distance threshold.
        if (app->waiting_second_tap_ && lv_tick_elaps(app->first_tap_tick_) <= DOUBLE_TAP_MS) {
            app->waiting_second_tap_ = false;
            app->exiting_ = true;
            ESP_LOGI("display_test", "LVGL double tap: returning to desktop");
            if (!app->back()) app->exiting_ = false;
            return;
        }
        app->first_tap_tick_ = lv_tick_get();
        app->waiting_second_tap_ = true;
    }
}
bool DisplayTest::pause()
{
    waiting_second_tap_ = false;
    if (timer_) lv_timer_pause(timer_);
    return true;
}
bool DisplayTest::resume()
{
    waiting_second_tap_ = false;
    exiting_ = false;
    if (timer_) { lv_timer_reset(timer_); lv_timer_resume(timer_); }
    return true;
}
bool DisplayTest::close()
{
    if (timer_) { lv_timer_delete(timer_); timer_ = nullptr; }
    if (root_) { lv_obj_delete(root_); root_ = nullptr; }
    waiting_second_tap_ = false;
    exiting_ = false;
    return true;
}
}
