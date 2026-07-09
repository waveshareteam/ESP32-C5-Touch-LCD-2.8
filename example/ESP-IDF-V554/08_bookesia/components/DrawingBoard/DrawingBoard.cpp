/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "DrawingBoard.hpp"
#include "lvgl.h"
#include "esp_brookesia.hpp"

#ifdef ESP_UTILS_LOG_TAG
#undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "BS:DrawingBoard"
#include "esp_lib_utils.h"

LV_IMG_DECLARE(image_draw);

#define APP_NAME "Drawing"

namespace esp_brookesia::apps
{
    DrawingBoard *DrawingBoard::_instance = nullptr;

    DrawingBoard *DrawingBoard::requestInstance(bool use_status_bar, bool use_navigation_bar)
    {
        ESP_UTILS_LOGD("requestInstance called");
        if (_instance == nullptr)
        {
            _instance = new DrawingBoard(use_status_bar, use_navigation_bar);
        }
        return _instance;
    }

    DrawingBoard::DrawingBoard(bool use_status_bar, bool use_navigation_bar)
        : App(APP_NAME, &image_draw, true, use_status_bar, use_navigation_bar)
    {
        ESP_UTILS_LOGD("Constructor called");
    }

    DrawingBoard::~DrawingBoard()
    {
        _instance = nullptr;
    }

    // 触摸回调：在触摸位置创建一个红色圆点
    void DrawingBoard::_touch_event_cb(lv_event_t *e)
    {
        lv_event_code_t code = lv_event_get_code(e);
        if (code != LV_EVENT_PRESSED && code != LV_EVENT_PRESSING) {
            return;
        }

        lv_indev_t *indev = lv_indev_get_act();
        if (indev == nullptr) return;

        lv_point_t point;
        lv_indev_get_point(indev, &point);

        // 获取当前活动屏幕
        lv_obj_t *screen = lv_scr_act();
        if (screen == nullptr) return;

        // 创建圆点对象（每次触摸新建，不会自动删除）
        lv_obj_t *dot = lv_obj_create(screen);
        lv_obj_set_size(dot, 10, 10);
        // 偏移位置与第一个示例一致：point.x - 5, point.y - 5 
        lv_obj_set_pos(dot, point.x - 5, point.y - 5 );
        lv_obj_set_style_bg_color(dot, lv_color_make(255, 0, 0), 0);
        lv_obj_set_style_radius(dot, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_border_width(dot, 0, 0);
    }

    bool DrawingBoard::run(void)
    {
        ESP_UTILS_LOGD("Run function started");

        // 获取可视区域（如有状态栏/导航栏会自动扣除）
        lv_area_t area = getVisualArea();
        int width = area.x2 - area.x1;
        int height = area.y2 - area.y1;

        // 创建一个全屏透明面板用于接收触摸事件
        lv_obj_t *touch_panel = lv_obj_create(lv_scr_act());
        lv_obj_set_size(touch_panel, width, height);
        lv_obj_set_pos(touch_panel, area.x1, area.y1);
        lv_obj_set_style_bg_color(touch_panel, lv_color_black(), 0);
        lv_obj_set_style_bg_opa(touch_panel, LV_OPA_TRANSP, 0);  // 透明背景
        lv_obj_set_style_border_width(touch_panel, 0, 0);
        lv_obj_clear_flag(touch_panel, LV_OBJ_FLAG_SCROLLABLE);

        // 注册触摸事件回调
        lv_obj_add_event_cb(touch_panel, _touch_event_cb, LV_EVENT_PRESSED, this);
        lv_obj_add_event_cb(touch_panel, _touch_event_cb, LV_EVENT_PRESSING, this);

        // 禁用屏幕滚动，防止拖动
        lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLLABLE);
        lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLL_CHAIN_HOR);
        lv_obj_clear_flag(lv_scr_act(), LV_OBJ_FLAG_SCROLL_CHAIN_VER);

        ESP_UTILS_LOGD("Run function finished successfully");
        return true;
    }

    bool DrawingBoard::back(void)
    {
        ESP_UTILS_LOGD("Back");
        ESP_UTILS_CHECK_FALSE_RETURN(notifyCoreClosed(), false, "Notify core closed failed");
        return true;
    }

    bool DrawingBoard::close(void)
    {
        ESP_UTILS_LOGD("Close");
        // 不需要释放画布缓冲区，因为未使用
        return true;
    }

    bool DrawingBoard::init()
    {
        ESP_UTILS_LOGD("Init");
        return true;
    }

    bool DrawingBoard::deinit()
    {
        ESP_UTILS_LOGD("Deinit");
        return true;
    }

    bool DrawingBoard::pause()
    {
        ESP_UTILS_LOGD("Pause");
        return true;
    }

    bool DrawingBoard::resume()
    {
        ESP_UTILS_LOGD("Resume");
        return true;
    }

} // namespace esp_brookesia::apps