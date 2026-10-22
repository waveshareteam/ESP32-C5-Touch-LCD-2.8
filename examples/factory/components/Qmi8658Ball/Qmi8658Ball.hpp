/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#pragma once
#include "systems/phone/esp_brookesia_phone_app.hpp"
// Import the shared sensor types through BSP once. qmi8658 v1.0.1 closes
// extern "C" outside its include guard and cannot be included twice in C++.
#include "bsp/esp-bsp.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lvgl.h" // 确保包含lvgl头文件

namespace esp_brookesia::apps
{
    class Qmi8658Ball : public systems::phone::App
    {
    public:
        static Qmi8658Ball *requestInstance(bool use_status_bar = false, bool use_navigation_bar = false);
        ~Qmi8658Ball();

        using systems::phone::App::endRecordResource;
        using systems::phone::App::startRecordResource;

    protected:
        Qmi8658Ball(bool use_status_bar, bool use_navigation_bar);
        bool run(void) override;
        bool back(void) override;
        bool close(void) override;
        bool init(void) override;
        bool deinit(void) override;
        bool pause(void) override;
        bool resume(void) override;

    private:
        static Qmi8658Ball *_instance;

        // LVGL UI元素（按初始化列表顺序排列）
        lv_obj_t *_ball = nullptr;
        lv_obj_t *_calib_label = nullptr;
        lv_obj_t *_calib_progress_label = nullptr;
        lv_obj_t *_calib_progress_bar = nullptr;
        lv_area_t _screen_area = {0};

        // 传感器和任务相关（按初始化列表顺序排列）
        qmi8658_dev_t *_qmi8658_dev;
        TaskHandle_t _update_task_handle;
        TaskHandle_t _calibration_task_handle; // 校准任务句柄
        i2c_master_bus_handle_t _i2c_bus;
        bool _calibration_done;
        bool _recalibration_requested;

        // 校准参数（按初始化列表顺序排列）
        float _accel_bias_x;
        float _accel_bias_y;
        int _display_width;
        int _display_height;

        enum CalibState
        {
            CALIB_IDLE,
            CALIB_RUNNING,
            CALIB_DONE,
            CALIB_FAILED
        } _calib_state;

        // 添加校准进度变量
        int _calib_progress;
        lv_timer_t *_calibration_ui_update_timer;

        // 物理参数
        const int BALL_RADIUS = 20;
        const int ACCEL_SCALE_FACTOR = 8;
        const int TASK_DELAY_MS = 30;
        const float CALIBRATION_DEADZONE = 0.05f;
        const float SCREEN_WIDTH_MM = 33.09f;
        const float SCREEN_HEIGHT_MM = 41.51f;
        const float CORNER_RADIUS_MM = 5.2f;
        const gpio_num_t CALIB_BUTTON_GPIO = GPIO_NUM_28;

        void update_calibration_ui(void);

        // 私有方法
        static void touch_event_cb(lv_event_t *e);
        static void ball_update_task(void *arg);
        static void calibration_task(void *arg); // 校准任务函数
        static void IRAM_ATTR button_isr_handler(void *arg);
        static void calibration_ui_update_timer_cb(lv_timer_t *timer);

        void init_calibration_button(void);
        void perform_level_calibration(void);
        void apply_calibration_and_deadzone(qmi8658_data_t *data);
        void constrain_to_rounded_rect(int *x, int *y);
        void create_ui_elements(void);
    };
} // namespace esp_brookesia::apps
