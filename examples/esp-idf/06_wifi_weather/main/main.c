/********************************************************************************************************
 * @file    main.c
 * @author  Javen
 * @company Waveshare
 * @date    2026/04/22
 * @brief   LVGL v9 Demo for Weather
 *
 * @note
 *          1. Connects to the network via Wi-Fi and retrieves real-time weather data for Shenzhen
 *          2. Calls a weather API to obtain temperature, humidity, wind speed, wind direction, and conditions
 *          3. Uses the LVGL v9 graphics library to display weather data on screen in real time
 *          4. Supports automatic Wi-Fi reconnection; retries continuously if the hotspot is not available
 *          5. Weather data is automatically refreshed every 5 minutes
 ********************************************************************************************************/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_memory_utils.h"
#include "lvgl.h"
#include "bsp/esp-bsp.h"
#include "bsp/display.h"

#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"


lv_ui guider_ui;


void app_main(void)
{

    bsp_display_start();
    bsp_display_backlight_off();

    bsp_display_lock(0);
    setup_ui(&guider_ui);
    custom_init(&guider_ui);
    events_init(&guider_ui);

    bsp_display_unlock();

    bsp_display_backlight_on();

}


