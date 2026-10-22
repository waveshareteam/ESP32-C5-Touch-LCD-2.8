/*
 * SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#include "Qmi8658Ball.hpp"
#include "lvgl.h"
#include "esp_brookesia.hpp"
#include "esp_lib_utils.h"
#include "bsp/esp-bsp.h"
#include "bsp/display.h"
#include "driver/gpio.h"
#include <cmath>

#ifdef ESP_UTILS_LOG_TAG
#undef ESP_UTILS_LOG_TAG
#endif
#define ESP_UTILS_LOG_TAG "BS:Qmi8658Ball"

LV_IMG_DECLARE(img_app_qmi8658ball);

namespace esp_brookesia::apps
{

    Qmi8658Ball *Qmi8658Ball::_instance = nullptr;

    Qmi8658Ball *Qmi8658Ball::requestInstance(bool use_status_bar, bool use_navigation_bar)
    {
        if (_instance == nullptr)
        {
            _instance = new Qmi8658Ball(use_status_bar, use_navigation_bar);
        }
        return _instance;
    }

    Qmi8658Ball::Qmi8658Ball(bool use_status_bar, bool use_navigation_bar) : App("GravityBall", &img_app_qmi8658ball, true, use_status_bar, use_navigation_bar),
                                                                             _ball(nullptr),
                                                                             _calib_label(nullptr),
                                                                             _calib_progress_label(nullptr),
                                                                             _calib_progress_bar(nullptr),
                                                                             _screen_area({0}),
                                                                             _qmi8658_dev(nullptr),
                                                                             _update_task_handle(nullptr),
                                                                             _calibration_task_handle(nullptr),
                                                                             _i2c_bus(nullptr),
                                                                             _calibration_done(false),
                                                                             _recalibration_requested(false),
                                                                             _accel_bias_x(0.0f),
                                                                             _accel_bias_y(0.0f),
                                                                             _display_width(0),
                                                                             _display_height(0),
                                                                             _calib_state(CALIB_IDLE),
                                                                             _calib_progress(0),
                                                                             _calibration_ui_update_timer(nullptr)
    {
    }

    Qmi8658Ball::~Qmi8658Ball()
    {
        if (_update_task_handle)
        {
            vTaskDelete(_update_task_handle);
            _update_task_handle = nullptr;
        }
        if (_qmi8658_dev)
        {
            free(_qmi8658_dev);
            _qmi8658_dev = nullptr;
        }
        gpio_isr_handler_remove(CALIB_BUTTON_GPIO);
    }

    bool Qmi8658Ball::init(void)
    {
        ESP_UTILS_LOGD("Init");

        _i2c_bus = bsp_i2c_get_handle();
        _qmi8658_dev = (qmi8658_dev_t *)malloc(sizeof(qmi8658_dev_t));
        ESP_ERROR_CHECK(qmi8658_init(_qmi8658_dev, _i2c_bus, QMI8658_ADDRESS_HIGH));

        qmi8658_set_accel_range(_qmi8658_dev, QMI8658_ACCEL_RANGE_8G);
        qmi8658_set_accel_odr(_qmi8658_dev, QMI8658_ACCEL_ODR_500HZ);
        qmi8658_set_accel_unit_mps2(_qmi8658_dev, true);
        qmi8658_write_register(_qmi8658_dev, QMI8658_CTRL5, 0x03);

        init_calibration_button();
        return true;
    }

    bool Qmi8658Ball::run(void)
    {
        ESP_UTILS_LOGD("Run");
        _screen_area = getVisualArea();
        _display_width = _screen_area.x2 - _screen_area.x1;
        _display_height = _screen_area.y2 - _screen_area.y1;

        create_ui_elements();

        _calib_state = CALIB_RUNNING;
        _calib_progress = 0;

        xTaskCreatePinnedToCore(
            calibration_task,
            "calibration_task",
            4096,
            this,
            5,
            &_calibration_task_handle,
            0);

        return true;
    }

    bool Qmi8658Ball::back(void)
    {
        ESP_UTILS_LOGD("Back");
        ESP_UTILS_CHECK_FALSE_RETURN(notifyCoreClosed(), false, "Notify core closed failed");
        return true;
    }

    bool Qmi8658Ball::close(void)
    {
        ESP_UTILS_LOGD("Close");
        if (_update_task_handle)
        {
            vTaskDelete(_update_task_handle);
            _update_task_handle = nullptr;
        }
        if (_ball)
        {
            lv_obj_del(_ball);
            _ball = nullptr;
        }
        if (_calib_label)
        {
            lv_obj_del(_calib_label);
            _calib_label = nullptr;
        }
        if (_calib_progress_label)
        {
            lv_obj_del(_calib_progress_label);
            _calib_progress_label = nullptr;
        }
        if (_calib_progress_bar)
        {
            lv_obj_del(_calib_progress_bar);
            _calib_progress_bar = nullptr;
        }
        if (_calibration_ui_update_timer)
        {
            lv_timer_del(_calibration_ui_update_timer);
            _calibration_ui_update_timer = nullptr;
        }
        return true;
    }

    bool Qmi8658Ball::deinit(void)
    {
        ESP_UTILS_LOGD("Deinit");
        return true;
    }

    bool Qmi8658Ball::pause(void)
    {
        ESP_UTILS_LOGD("Pause");
        if (_update_task_handle)
        {
            vTaskSuspend(_update_task_handle);
        }
        return true;
    }

    bool Qmi8658Ball::resume(void)
    {
        ESP_UTILS_LOGD("Resume");
        if (_update_task_handle)
        {
            vTaskResume(_update_task_handle);
        }
        return true;
    }

    void Qmi8658Ball::create_ui_elements(void)
    {
        bsp_display_lock(pdMS_TO_TICKS(200));

        _calib_progress_bar = lv_bar_create(lv_scr_act());
        lv_obj_set_size(_calib_progress_bar, 200, 20);
        lv_obj_align(_calib_progress_bar, LV_ALIGN_CENTER, 0, -20);
        lv_bar_set_range(_calib_progress_bar, 0, 100);

        _calib_progress_label = lv_label_create(lv_scr_act());
        lv_label_set_text(_calib_progress_label, "Calibrating... 0%");
        lv_obj_align_to(_calib_progress_label, _calib_progress_bar, LV_ALIGN_OUT_BOTTOM_MID, 0, 10);

        _calib_label = lv_label_create(lv_scr_act());
        lv_label_set_text(_calib_label, "Press BOOT button to recalibrate");
        lv_obj_set_style_text_font(_calib_label, &lv_font_montserrat_12, 0);
        lv_obj_align(_calib_label, LV_ALIGN_TOP_MID, 0, 10);
        lv_obj_add_flag(_calib_label, LV_OBJ_FLAG_HIDDEN);

        _ball = lv_obj_create(lv_scr_act());
        lv_obj_set_size(_ball, BALL_RADIUS * 2, BALL_RADIUS * 2);
        lv_obj_set_style_radius(_ball, LV_RADIUS_CIRCLE, 0);
        lv_obj_set_style_bg_color(_ball, lv_color_hex(0x006400), 0);
        lv_obj_set_style_border_width(_ball, 0, 0);
        lv_obj_set_style_shadow_width(_ball, 5, 0);
        int center_x = _screen_area.x1 + _display_width / 2;
        int center_y = _screen_area.y1 + _display_height / 2;
        lv_obj_set_pos(_ball, center_x - BALL_RADIUS, center_y - BALL_RADIUS);
        lv_obj_add_flag(_ball, LV_OBJ_FLAG_HIDDEN);

        bsp_display_unlock();
    }

    void Qmi8658Ball::calibration_task(void *arg)
    {
        Qmi8658Ball *app = static_cast<Qmi8658Ball *>(arg);
        app->perform_level_calibration();
    }

    void Qmi8658Ball::init_calibration_button(void)
    {
        gpio_config_t io_conf = {
            .pin_bit_mask = (1ULL << CALIB_BUTTON_GPIO),
            .mode = GPIO_MODE_INPUT,
            .pull_up_en = GPIO_PULLUP_ENABLE,
            .pull_down_en = GPIO_PULLDOWN_DISABLE,
            .intr_type = GPIO_INTR_NEGEDGE};
        gpio_config(&io_conf);

        gpio_install_isr_service(0);
        gpio_isr_handler_add(CALIB_BUTTON_GPIO, button_isr_handler, this);
    }

    void Qmi8658Ball::perform_level_calibration(void)
    {
        qmi8658_data_t data;
        const int CALIB_SAMPLES = 200;
        float sum_x = 0.0f, sum_y = 0.0f;
        float max_x = -10.0f, min_x = 10.0f;
        float max_y = -10.0f, min_y = 10.0f;

        ESP_UTILS_LOGI("Starting level calibration...");

        if (_calibration_ui_update_timer == nullptr)
        {
            _calibration_ui_update_timer = lv_timer_create(calibration_ui_update_timer_cb, 100, this);
        }

        for (int i = 0; i < CALIB_SAMPLES; i++)
        {
            if (qmi8658_read_sensor_data(_qmi8658_dev, &data) == ESP_OK)
            {
                sum_x += data.accelX;
                sum_y += data.accelY;

                max_x = fmaxf(max_x, data.accelX);
                min_x = fminf(min_x, data.accelX);
                max_y = fmaxf(max_y, data.accelY);
                min_y = fminf(min_y, data.accelY);
            }

            if (i % 10 == 0)
            {
                _calib_progress = (i * 100) / CALIB_SAMPLES;
            }

            vTaskDelay(pdMS_TO_TICKS(10));
        }

        float range_x = max_x - min_x;
        float range_y = max_y - min_y;

        if (range_x > 0.1f || range_y > 0.1f)
        {
            ESP_UTILS_LOGW("Calibration unstable (X: %.4f, Y: %.4f)", range_x, range_y);
            _calib_state = CALIB_FAILED;
            _calibration_done = false;
        }
        else
        {
            _accel_bias_x = sum_x / CALIB_SAMPLES;
            _accel_bias_y = sum_y / CALIB_SAMPLES;
            _calib_state = CALIB_DONE;
            _calibration_done = true;
            ESP_UTILS_LOGI("Calibration complete. Bias X: %.4f, Y: %.4f", _accel_bias_x, _accel_bias_y);
        }

        _calibration_task_handle = nullptr;
        vTaskDelete(NULL);

        if (_calibration_ui_update_timer)
        {
            lv_timer_del(_calibration_ui_update_timer);
            _calibration_ui_update_timer = nullptr;
        }
    }

    void Qmi8658Ball::update_calibration_ui(void)
    {
        bsp_display_lock(pdMS_TO_TICKS(50));

        switch (_calib_state)
        {
        case CALIB_RUNNING:
            if (_calib_progress_bar)
                lv_bar_set_value(_calib_progress_bar, _calib_progress, LV_ANIM_OFF);
            if (_calib_progress_label)
                lv_label_set_text_fmt(_calib_progress_label, "Calibrating... %d%%", _calib_progress);
            break;

        case CALIB_DONE:
            if (_calib_progress_label)
                lv_label_set_text(_calib_progress_label, "Calibration complete!");

            if (_ball)
                lv_obj_clear_flag(_ball, LV_OBJ_FLAG_HIDDEN);
            if (_calib_label)
                lv_obj_clear_flag(_calib_label, LV_OBJ_FLAG_HIDDEN);

            if (_calib_progress_bar)
                lv_obj_add_flag(_calib_progress_bar, LV_OBJ_FLAG_HIDDEN);
            if (_calib_progress_label)
                lv_obj_add_flag(_calib_progress_label, LV_OBJ_FLAG_HIDDEN);

            if (!_update_task_handle && _ball)
            {
                xTaskCreatePinnedToCore(
                    ball_update_task,
                    "ball_update",
                    8192,
                    this,
                    3,
                    &_update_task_handle,
                    0);
            }
            break;

        case CALIB_FAILED:
            if (_calib_progress_label)
                lv_label_set_text(_calib_progress_label, "Calibration failed! Retrying...");
            bsp_display_unlock();
            vTaskDelay(pdMS_TO_TICKS(1000));
            _calib_state = CALIB_RUNNING;
            _calib_progress = 0;
            xTaskCreatePinnedToCore(
                calibration_task,
                "calibration_retry",
                4096,
                this,
                5,
                &_calibration_task_handle,
                0);
            return;

        default:
            break;
        }

        bsp_display_unlock();
    }

    void Qmi8658Ball::apply_calibration_and_deadzone(qmi8658_data_t *data)
    {
        if (_calibration_done)
        {
            data->accelX -= _accel_bias_x;
            data->accelY -= _accel_bias_y;

            if (fabsf(data->accelX) < CALIBRATION_DEADZONE)
                data->accelX = 0.0f;
            if (fabsf(data->accelY) < CALIBRATION_DEADZONE)
                data->accelY = 0.0f;
        }
    }

    void Qmi8658Ball::constrain_to_rounded_rect(int *x, int *y)
    {
        const int screen_width = _display_width;
        const int screen_height = _display_height;

        if (*x < BALL_RADIUS)
            *x = BALL_RADIUS;
        if (*x > screen_width - BALL_RADIUS)
            *x = screen_width - BALL_RADIUS;
        if (*y < BALL_RADIUS)
            *y = BALL_RADIUS;
        if (*y > screen_height - BALL_RADIUS)
            *y = screen_height - BALL_RADIUS;

        float px_per_mm_x = (float)screen_width / SCREEN_WIDTH_MM;
        float px_per_mm_y = (float)screen_height / SCREEN_HEIGHT_MM;
        float px_per_mm = fminf(px_per_mm_x, px_per_mm_y);
        int corner_radius_px = (int)(CORNER_RADIUS_MM * px_per_mm);

        if (corner_radius_px < BALL_RADIUS)
        {
            corner_radius_px = BALL_RADIUS + 5;
        }

        const int safe_left = corner_radius_px;
        const int safe_right = screen_width - corner_radius_px;
        const int safe_top = corner_radius_px;
        const int safe_bottom = screen_height - corner_radius_px;

        int corner_radius_effective = corner_radius_px - BALL_RADIUS;

        if (*x < safe_left && *y < safe_top)
        {
            float dx = safe_left - *x;
            float dy = safe_top - *y;
            float dist = sqrtf(dx * dx + dy * dy);

            if (dist > corner_radius_effective && corner_radius_effective > 0)
            {
                float ratio = corner_radius_effective / dist;
                *x = safe_left - (int)(dx * ratio);
                *y = safe_top - (int)(dy * ratio);
            }
        }
        else if (*x > safe_right && *y < safe_top)
        {
            float dx = *x - safe_right;
            float dy = safe_top - *y;
            float dist = sqrtf(dx * dx + dy * dy);

            if (dist > corner_radius_effective && corner_radius_effective > 0)
            {
                float ratio = corner_radius_effective / dist;
                *x = safe_right + (int)(dx * ratio);
                *y = safe_top - (int)(dy * ratio);
            }
        }
        else if (*x < safe_left && *y > safe_bottom)
        {
            float dx = safe_left - *x;
            float dy = *y - safe_bottom;
            float dist = sqrtf(dx * dx + dy * dy);

            if (dist > corner_radius_effective && corner_radius_effective > 0)
            {
                float ratio = corner_radius_effective / dist;
                *x = safe_left - (int)(dx * ratio);
                *y = safe_bottom + (int)(dy * ratio);
            }
        }
        else if (*x > safe_right && *y > safe_bottom)
        {
            float dx = *x - safe_right;
            float dy = *y - safe_bottom;
            float dist = sqrtf(dx * dx + dy * dy);

            if (dist > corner_radius_effective && corner_radius_effective > 0)
            {
                float ratio = corner_radius_effective / dist;
                *x = safe_right + (int)(dx * ratio);
                *y = safe_bottom + (int)(dy * ratio);
            }
        }
    }

    void IRAM_ATTR Qmi8658Ball::button_isr_handler(void *arg)
    {
        Qmi8658Ball *app = static_cast<Qmi8658Ball *>(arg);
        if (app)
        {
            ESP_UTILS_LOGD("Calibration button pressed!");
            app->_recalibration_requested = true;
        }
    }

    void Qmi8658Ball::ball_update_task(void *arg)
    {
        Qmi8658Ball *app = static_cast<Qmi8658Ball *>(arg);
        qmi8658_data_t data;

        while (app->_calib_state != CALIB_DONE)
        {
            vTaskDelay(pdMS_TO_TICKS(100));
        }

        int rel_x = app->_display_width / 2;
        int rel_y = app->_display_height / 2;

        while (1)
        {
            if (app->_recalibration_requested)
            {
                app->_recalibration_requested = false;

                bsp_display_lock(pdMS_TO_TICKS(50));
                lv_obj_add_flag(app->_ball, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(app->_calib_progress_bar, LV_OBJ_FLAG_HIDDEN);
                lv_obj_clear_flag(app->_calib_progress_label, LV_OBJ_FLAG_HIDDEN);
                bsp_display_unlock();

                app->_calib_state = CALIB_RUNNING;
                app->_calib_progress = 0;
                xTaskCreatePinnedToCore(
                    calibration_task,
                    "calibration_recalib",
                    4096,
                    app,
                    5,
                    &app->_calibration_task_handle,
                    0);

                while (app->_calib_state != CALIB_DONE)
                {
                    vTaskDelay(pdMS_TO_TICKS(100));
                }

                bsp_display_lock(pdMS_TO_TICKS(50));
                lv_obj_clear_flag(app->_ball, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(app->_calib_progress_bar, LV_OBJ_FLAG_HIDDEN);
                lv_obj_add_flag(app->_calib_progress_label, LV_OBJ_FLAG_HIDDEN);
                bsp_display_unlock();

                rel_x = app->_display_width / 2;
                rel_y = app->_display_height / 2;
                int center_x = app->_screen_area.x1 + rel_x;
                int center_y = app->_screen_area.y1 + rel_y;
                bsp_display_lock(pdMS_TO_TICKS(50));
                lv_obj_set_pos(app->_ball, center_x - app->BALL_RADIUS, center_y - app->BALL_RADIUS);
                bsp_display_unlock();
            }

            bool ready;
            esp_err_t ret = qmi8658_is_data_ready(app->_qmi8658_dev, &ready);
            if (ret != ESP_OK || !ready)
            {
                vTaskDelay(pdMS_TO_TICKS(app->TASK_DELAY_MS));
                continue;
            }

            ret = qmi8658_read_sensor_data(app->_qmi8658_dev, &data);
            if (ret == ESP_OK)
            {
                app->apply_calibration_and_deadzone(&data);

                // 修改为 ball_qmi.c 中的运动逻辑：X 用 accelY，Y 用 -accelX
                int move_x = (int)(data.accelY * app->ACCEL_SCALE_FACTOR);
                int move_y = -(int)(data.accelX * app->ACCEL_SCALE_FACTOR);
                rel_x += move_x;
                rel_y += move_y;

                app->constrain_to_rounded_rect(&rel_x, &rel_y);

                int abs_x = app->_screen_area.x1 + rel_x;
                int abs_y = app->_screen_area.y1 + rel_y;

                bsp_display_lock(pdMS_TO_TICKS(50));
                lv_obj_set_pos(app->_ball, abs_x - app->BALL_RADIUS, abs_y - app->BALL_RADIUS);
                bsp_display_unlock();
            }

            vTaskDelay(pdMS_TO_TICKS(app->TASK_DELAY_MS));
        }
    }

    void Qmi8658Ball::calibration_ui_update_timer_cb(lv_timer_t *timer)
    {
        Qmi8658Ball *app = static_cast<Qmi8658Ball *>(timer->user_data);
        app->update_calibration_ui();
    }

} // namespace esp_brookesia::apps