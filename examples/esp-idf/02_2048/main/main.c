/********************************************************************************************************
 * @file    main.c
 * @author  Javen
 * @company Waveshare
 * @date    2026/04/22
 * @brief   LVGL v9 Demo for 2048 Mini Game
 *
 * @note
 *          1. 2048 mini game ported based on LVGL v9 framework
 *          2. Interactive demonstration of 2048 game with basic control logic
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
#include "lv_demos.h"

#include "gui_guider.h"
#include "events_init.h"
#include "widgets_init.h"
#include "custom.h"

static const char *TAG = "MAIN";   

lv_ui guider_ui;



void app_main(void)
{

    bsp_display_start();
    
    bsp_display_backlight_off();

    // vTaskDelay(pdMS_TO_TICKS(1000));

    bsp_display_lock(-1);

    setup_ui(&guider_ui);
    custom_init(&guider_ui);
    events_init(&guider_ui);
    // lv_demo_music();

    bsp_display_unlock();

    bsp_display_backlight_on();



}