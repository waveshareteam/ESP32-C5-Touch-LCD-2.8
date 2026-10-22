#include "sensor_init.h"

#include <stdbool.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "lvgl.h"
#include "bsp/esp-bsp.h"
#include "bsp/display.h"
#include "gui_guider.h"
#include "events_init.h"
#include "custom.h"

static const char *TAG = "Sensor";

lv_ui sensor_guider_ui;

extern qmi8658_dev_t qmi_dev;
extern pcf85063a_dev_t pcf_dev;
extern i2c_master_dev_handle_t shtc3_handle;

static SemaphoreHandle_t s_i2c_bus_mutex = NULL;
static TaskHandle_t s_qmi_task_handle = NULL;
static TaskHandle_t s_pcf_task_handle = NULL;
static TaskHandle_t s_shtc3_task_handle = NULL;
static bool s_sensor_ui_initialized = false;

static void delete_inactive_sensor_screen(void)
{
    if (sensor_guider_ui.start_screen == NULL || !lv_obj_is_valid(sensor_guider_ui.start_screen)) {
        sensor_guider_ui.start_screen = NULL;
        return;
    }

    if (sensor_guider_ui.start_screen != lv_screen_active()) {
        lv_obj_delete(sensor_guider_ui.start_screen);
        sensor_guider_ui.start_screen = NULL;
    }
}

static void qmi_task(void *arg)
{
    qmi8658_dev_t *dev = (qmi8658_dev_t *)arg;
    qmi8658_data_t data;
    char x_buf[32];
    char y_buf[32];
    char z_buf[32];

    vTaskDelay(pdMS_TO_TICKS(500));

    while (1) {
        bool read_ok = false;

        if (xSemaphoreTake(s_i2c_bus_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            bool ready = false;
            esp_err_t ret = qmi8658_is_data_ready(dev, &ready);
            if (ret == ESP_OK && ready) {
                read_ok = (qmi8658_read_sensor_data(dev, &data) == ESP_OK);
                if (!read_ok) {
                    ESP_LOGW(TAG, "Failed to read QMI8658 sensor data");
                }
            } else if (ret != ESP_OK) {
                ESP_LOGW(TAG, "Failed to read QMI8658 data-ready status");
            }
            xSemaphoreGive(s_i2c_bus_mutex);
        }

        if (read_ok) {
            snprintf(x_buf, sizeof(x_buf), "%.3f", data.gyroX);
            snprintf(y_buf, sizeof(y_buf), "%.3f", data.gyroY);
            snprintf(z_buf, sizeof(z_buf), "%.3f", data.gyroZ);

            bsp_display_lock(-1);
            lv_label_set_text(sensor_guider_ui.start_screen_x_lab2, x_buf);
            lv_label_set_text(sensor_guider_ui.start_screen_y_lab2, y_buf);
            lv_label_set_text(sensor_guider_ui.start_screen_z_lab2, z_buf);
            bsp_display_unlock();
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

static void pcf85063a_task(void *arg)
{
    pcf85063a_datetime_t now_time;
    char date_buf[32];
    char time_buf[32];

    while (1) {
        bool read_ok = false;

        if (xSemaphoreTake(s_i2c_bus_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            read_ok = (pcf85063a_get_time_date(&pcf_dev, &now_time) == ESP_OK);
            if (!read_ok) {
                ESP_LOGW(TAG, "Failed to read PCF85063A time data");
            }
            xSemaphoreGive(s_i2c_bus_mutex);
        }

        if (read_ok) {
            snprintf(date_buf, sizeof(date_buf), "%04d.%02d.%02d", now_time.year, now_time.month, now_time.day);
            snprintf(time_buf, sizeof(time_buf), "%02d:%02d:%02d", now_time.hour, now_time.min, now_time.sec);

            bsp_display_lock(-1);
            lv_label_set_text(sensor_guider_ui.start_screen_data_lab, date_buf);
            lv_label_set_text(sensor_guider_ui.start_screen_time_lab, time_buf);
            bsp_display_unlock();
        }

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void shtc3_read_task(void *pvParameters)
{
    float temperature;
    float humidity;
    shtc3_register_rw_t reg = SHTC3_REG_T_CSE_NM;
    char temp_buf[32];
    char humid_buf[32];

    vTaskDelay(pdMS_TO_TICKS(1000));

    while (1) {
        bool read_ok = false;

        if (xSemaphoreTake(s_i2c_bus_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            esp_err_t err = shtc3_get_th(shtc3_handle, reg, &temperature, &humidity);
            if (err != ESP_OK) {
                ESP_LOGE(TAG, "Failed to read data from SHTC3 sensor");
            } else {
                read_ok = (temperature > -40.0f && temperature < 125.0f &&
                           humidity >= 0.0f && humidity <= 100.0f);
                if (!read_ok) {
                    ESP_LOGW(TAG, "SHTC3 invalid data: temp=%.2f, humi=%.2f, skipped",
                             temperature, humidity);
                }
            }
            xSemaphoreGive(s_i2c_bus_mutex);
        }

        if (read_ok) {
            snprintf(temp_buf, sizeof(temp_buf), "%.2f C", temperature);
            snprintf(humid_buf, sizeof(humid_buf), "%.2f %%", humidity);

            bsp_display_lock(-1);
            lv_label_set_text(sensor_guider_ui.start_screen_wendu_lab, temp_buf);
            lv_label_set_text(sensor_guider_ui.start_screen_shidu_lab, humid_buf);
            bsp_display_unlock();
        }

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

static void stop_task(TaskHandle_t *task)
{
    if (*task != NULL) {
        vTaskDelete(*task);
        *task = NULL;
    }
}

void init_sensor_ui(void)
{
    if (s_sensor_ui_initialized) {
        return;
    }

    bsp_display_lock(-1);
    delete_inactive_sensor_screen();
    sensor_setup_ui(&sensor_guider_ui);
    sensor_custom_init(&sensor_guider_ui);
    sensor_events_init(&sensor_guider_ui);
    bsp_display_unlock();

    s_i2c_bus_mutex = xSemaphoreCreateMutex();
    if (s_i2c_bus_mutex == NULL) {
        ESP_LOGE(TAG, "Create I2C bus mutex failed");
        return;
    }

    if (pcf_init() != ESP_OK) {
        ESP_LOGE(TAG, "PCF85063A init failed");
    }
    if (shtc3_init() != ESP_OK) {
        ESP_LOGE(TAG, "SHTC3 init failed");
    }
    if (qmi_init() != ESP_OK) {
        ESP_LOGE(TAG, "QMI8658 init failed");
    }

    xTaskCreatePinnedToCore(pcf85063a_task, "sensor_pcf85063a", 4096, NULL, 1, &s_pcf_task_handle, 0);
    xTaskCreatePinnedToCore(shtc3_read_task, "sensor_shtc3", 4096, NULL, 2, &s_shtc3_task_handle, 0);
    xTaskCreatePinnedToCore(qmi_task, "sensor_qmi8658", 4096, &qmi_dev, 1, &s_qmi_task_handle, 0);

    s_sensor_ui_initialized = true;
}

void deinit_sensor_ui(void)
{
    stop_task(&s_qmi_task_handle);
    stop_task(&s_pcf_task_handle);
    stop_task(&s_shtc3_task_handle);

    if (s_i2c_bus_mutex != NULL) {
        vSemaphoreDelete(s_i2c_bus_mutex);
        s_i2c_bus_mutex = NULL;
    }

    s_sensor_ui_initialized = false;
}
