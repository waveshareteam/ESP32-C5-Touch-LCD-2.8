/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 * SPDX-License-Identifier: Apache-2.0
 * Adapted from the C5/RLCD DrawingBoard APP for the 240x320 RGB565 display.
 */
#include "DrawingBoard.hpp"
#include "bsp/esp-bsp.h"
#include "esp_heap_caps.h"
#include "src/misc/cache/instance/lv_image_cache.h"
#include <new>

LV_IMAGE_DECLARE(image_draw);
namespace {
constexpr int WIDTH = BSP_LCD_H_RES, HEIGHT = BSP_LCD_V_RES, BRUSH = 10;
esp_brookesia::systems::phone::App::Config fullscreen_config()
{
    auto config = esp_brookesia::systems::phone::App::Config::SIMPLE_CONSTRUCTOR(&image_draw, false, false);
    // Edge swipes must remain drawable, including the bottom home-gesture area.
    // This flag only affects Touch Test. The back button and BOOT provide exits.
    config.flags.enable_navigation_gesture = false;
    return config;
}
}

namespace esp_brookesia::apps {
DrawingBoard *DrawingBoard::instance_;
DrawingBoard *DrawingBoard::requestInstance()
{
    if (!instance_) instance_ = new (std::nothrow) DrawingBoard();
    return instance_;
}
DrawingBoard::DrawingBoard()
    : App(systems::base::App::Config::SIMPLE_CONSTRUCTOR("Touch Test", &image_draw, true), fullscreen_config()) {}
DrawingBoard::~DrawingBoard() { instance_ = nullptr; }
bool DrawingBoard::init() { return true; }

bool DrawingBoard::run()
{
    const auto area = getVisualArea();
    if (lv_area_get_width(&area) != WIDTH || lv_area_get_height(&area) != HEIGHT) return false;
    const size_t size = LV_DRAW_BUF_SIZE(WIDTH, HEIGHT, LV_COLOR_FORMAT_RGB565);
    pixels_ = static_cast<uint8_t *>(heap_caps_aligned_alloc(LV_DRAW_BUF_ALIGN, size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT));
    if (!pixels_) return false;
    if (lv_draw_buf_init(&draw_buffer_, WIDTH, HEIGHT, LV_COLOR_FORMAT_RGB565,
                        LV_DRAW_BUF_STRIDE(WIDTH, LV_COLOR_FORMAT_RGB565), pixels_, size) != LV_RESULT_OK) {
        heap_caps_free(pixels_); pixels_ = nullptr; return false;
    }
    lv_draw_buf_set_flag(&draw_buffer_, LV_IMAGE_FLAGS_MODIFIABLE);
    root_ = lv_obj_create(lv_screen_active());
    lv_obj_remove_style_all(root_);
    lv_obj_set_pos(root_, area.x1, area.y1); lv_obj_set_size(root_, WIDTH, HEIGHT);
    lv_obj_set_style_bg_color(root_, lv_color_white(), 0);
    lv_obj_set_style_bg_opa(root_, LV_OPA_COVER, 0);
    lv_obj_remove_flag(root_, LV_OBJ_FLAG_SCROLLABLE);
    canvas_ = lv_canvas_create(root_);
    lv_obj_remove_style_all(canvas_);
    lv_canvas_set_draw_buf(canvas_, &draw_buffer_);
    lv_obj_set_pos(canvas_, 0, 0);
    lv_obj_remove_flag(canvas_, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_remove_flag(canvas_, LV_OBJ_FLAG_SCROLL_CHAIN);
    lv_obj_remove_flag(canvas_, LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_flag(canvas_, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(canvas_, LV_OBJ_FLAG_PRESS_LOCK);
    lv_obj_add_event_cb(canvas_, touch_event, LV_EVENT_ALL, this);
    clear();

    // Create after the canvas so the return control stays above the drawing.
    // It is a sibling of the canvas; tapping it never generates a drawing dot.
    auto *back_button = lv_button_create(root_);
    lv_obj_remove_style_all(back_button);
    lv_obj_set_pos(back_button, 12, 14);
    lv_obj_set_size(back_button, 44, 40);
    lv_obj_set_style_radius(back_button, 12, 0);
    lv_obj_set_style_bg_color(back_button, lv_color_hex(0xf3f6fb), 0);
    lv_obj_set_style_bg_opa(back_button, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_color(back_button, lv_color_hex(0xdbe7ff), LV_STATE_PRESSED);
    lv_obj_set_style_border_color(back_button, lv_color_hex(0xd5deeb), 0);
    lv_obj_set_style_border_width(back_button, 1, 0);
    lv_obj_remove_flag(back_button, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_SCROLL_CHAIN | LV_OBJ_FLAG_GESTURE_BUBBLE);
    lv_obj_add_event_cb(back_button, back_event, LV_EVENT_CLICKED, this);
    auto *arrow = lv_label_create(back_button);
    lv_label_set_text(arrow, LV_SYMBOL_LEFT);
    lv_obj_set_style_text_font(arrow, &lv_font_montserrat_20, 0);
    lv_obj_set_style_text_color(arrow, lv_color_hex(0x376df6), 0);
    lv_obj_remove_flag(arrow, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_center(arrow);

    button_presses_ = bsp_user_button_get_press_count();
    exit_timer_ = lv_timer_create(poll_exit, 50, this);
    if (!exit_timer_) { close(); return false; }
    return true;
}

void DrawingBoard::clear()
{
    stroke_ = false;
    if (!canvas_) return;
    lv_canvas_fill_bg(canvas_, lv_color_white(), LV_OPA_COVER);
    lv_image_cache_drop(&draw_buffer_);
}

void DrawingBoard::draw(const lv_point_t &point)
{
    if (stroke_ && previous_.x == point.x && previous_.y == point.y) return;
    lv_layer_t layer;
    lv_canvas_init_layer(canvas_, &layer);
    // C5-2.8 behavior: one red dot per sampled point, no interpolation.
    // A fixed canvas avoids accumulating thousands of LVGL objects.
    {
        lv_draw_rect_dsc_t dot;
        lv_draw_rect_dsc_init(&dot);
        dot.bg_color = lv_color_hex(0xff0000);
        dot.bg_opa = LV_OPA_COVER;
        dot.radius = LV_RADIUS_CIRCLE;
        lv_area_t bounds = {point.x - BRUSH / 2, point.y - BRUSH / 2,
                            point.x + BRUSH / 2 - 1, point.y + BRUSH / 2 - 1};
        lv_draw_rect(&layer, &dot, &bounds);
    }
    lv_canvas_finish_layer(canvas_, &layer);
    lv_image_cache_drop(&draw_buffer_);
    previous_ = point;
    stroke_ = true;
}

void DrawingBoard::touch_event(lv_event_t *event)
{
    auto *app = static_cast<DrawingBoard *>(lv_event_get_user_data(event));
    auto code = lv_event_get_code(event);
    if (code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) { app->stroke_ = false; return; }
    if (code != LV_EVENT_PRESSED && code != LV_EVENT_PRESSING) return;
    auto *input = lv_indev_active();
    if (!input || !app->canvas_) return;
    if (code == LV_EVENT_PRESSED) app->stroke_ = false;
    lv_point_t point;
    lv_indev_get_point(input, &point);
    lv_area_t bounds;
    lv_obj_get_coords(app->canvas_, &bounds);
    point.x -= bounds.x1; point.y -= bounds.y1;
    if (point.x < 0 || point.x >= WIDTH || point.y < 0 || point.y >= HEIGHT) { app->stroke_ = false; return; }
    app->draw(point);
}
void DrawingBoard::back_event(lv_event_t *event)
{
    static_cast<DrawingBoard *>(lv_event_get_user_data(event))->back();
}

void DrawingBoard::poll_exit(lv_timer_t *timer)
{
    auto *app = static_cast<DrawingBoard *>(lv_timer_get_user_data(timer));
    const uint32_t presses = bsp_user_button_get_press_count();
    if (presses != app->button_presses_) {
        app->button_presses_ = presses;
        app->back();
    }
}
bool DrawingBoard::back() { stroke_ = false; return notifyCoreClosed(); }
bool DrawingBoard::close()
{
    stroke_ = false;
    if (exit_timer_) { lv_timer_delete(exit_timer_); exit_timer_ = nullptr; }
    if (root_) lv_obj_delete(root_);
    root_ = canvas_ = nullptr;
    if (pixels_) {
        lv_image_cache_drop(&draw_buffer_);
        heap_caps_free(pixels_); pixels_ = nullptr;
    }
    return true;
}
bool DrawingBoard::deinit() { return close(); }
bool DrawingBoard::pause()
{
    stroke_ = false;
    if (exit_timer_) lv_timer_pause(exit_timer_);
    return true;
}
bool DrawingBoard::resume()
{
    stroke_ = false;
    button_presses_ = bsp_user_button_get_press_count();
    if (exit_timer_) lv_timer_resume(exit_timer_);
    return true;
}
}
