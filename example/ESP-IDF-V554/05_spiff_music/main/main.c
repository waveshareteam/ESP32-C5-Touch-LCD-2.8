/********************************************************************************************************
 * @file    main.c
 * @author  Javen
 * @company Waveshare
 * @date    2026/04/22
 * @brief   LVGL v9 Demo for Music Player
 *
 * @note
 *          1. Scans MP3 music files in the SPIFFS file system /music directory
 *          2. Supports one-click play/pause control of music with maximum volume by default
 *          3. Uses LVGL v9 graphics library to realize the infinite rotation animation of music cover
 *          4. Automatically detects playback status and resets the UI when music finishes playing
 *          5. Supports resuming rotation animation from the pause angle during playback
 ********************************************************************************************************/

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_err.h"
#include "lvgl.h"
#include "bsp/esp-bsp.h"
#include "bsp/display.h"

#include "gui_guider.h"
#include "events_init.h"
#include "custom.h"

lv_ui guider_ui;

static const char *TAG = "MAIN";

void app_main(void)
{

    bsp_io_expander_init();
    vTaskDelay(pdMS_TO_TICKS(100));
    bsp_spiffs_mount();

    bsp_display_start();
    bsp_display_backlight_on();

    bsp_display_lock(-1);
    setup_ui(&guider_ui);
    custom_init(&guider_ui);
    events_init(&guider_ui);  
    bsp_display_unlock();

    ESP_LOGI(TAG, "System start success!");
    while (1) {
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(1));
    }

}