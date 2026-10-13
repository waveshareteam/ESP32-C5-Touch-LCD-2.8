/********************************************************************************************************
 * @file    main.c
 * @author  Javen
 * @company Waveshare
 * @date    2026/03/09
 * @brief   ESP32 QMI8658 Accelerometer Controlled LVGL Ball Application
 * 
 * @note    This application implements a gravity-controlled ball on LVGL display:
 *          1. Initializes QMI8658 accelerometer (8G range, 500Hz ODR, m/s² unit)
 *          2. Performs accelerometer level calibration to eliminate offset
 *          3. Creates a task to update ball position based on acceleration data
 *          4. Constrains ball movement within screen rounded corner boundaries
 ********************************************************************************************************/
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_err.h"
#include "lvgl.h"
#include "bsp/esp-bsp.h"
#include "bsp/display.h"

#include "ball_qmi.h"


void app_main(void)
{

    bsp_display_start();
    bsp_display_backlight_on();
    bsp_display_lock(0);
    bsp_display_unlock();

    i2c_master_bus_handle_t bus_handle = bsp_i2c_get_handle();
    qmi8658_dev_t *dev = malloc(sizeof(qmi8658_dev_t));
    ESP_ERROR_CHECK(qmi8658_init(dev, bus_handle, QMI8658_ADDRESS_HIGH));

    qmi8658_set_accel_range(dev, QMI8658_ACCEL_RANGE_8G);
    qmi8658_set_accel_odr(dev, QMI8658_ACCEL_ODR_500HZ);
    qmi8658_set_accel_unit_mps2(dev, true);
    qmi8658_write_register(dev, QMI8658_CTRL5, 0x03);

    ball_qmi_calibrate(dev);

    xTaskCreatePinnedToCore(
        ball_qmi_task, 
        "ball_qmi_task", 
        4064, 
        dev, 
        3, 
        NULL, 
        0
    );
}