/********************************************************************************************************
 * @file    main.c
 * @author  Javen
 * @company Waveshare
 * @date    2026-04-22
 * @brief   ESP32-C5-Touch-LCD-2.8 BSP SD Card & LVGL Display Demo
 *
 * @note    This demo implements SD card file reading and LVGL display based on official BSP drivers.
 *          1. Use BSP APIs for SD card mount/unmount operations
 *          2. Read text from Test.txt file in the SD card root directory
 *          3. Display file content on the LCD screen via LVGL
 *          4. SD card requirement: FAT32 format, maximum capacity 64GB
 ********************************************************************************************************/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_err.h"
#include "lvgl.h"
#include "bsp/esp-bsp.h"
#include "bsp/display.h"
#include "string.h"
#include "stdio.h"
#include "dirent.h"

#define TAG "SDCARD_LVGL_DEMO"
#define MAX_TEXT_LEN 1024  

static lv_obj_t *sd_text_label = NULL;

/**
 * @brief  Update LVGL screen text content
 * @param  text: String to display, default prompt will be shown if NULL
 * @return None
 */
void lvgl_update_sd_text(const char *text)
{
    if (sd_text_label == NULL) {
        ESP_LOGE(TAG, "sd_text_label init error");
        return;
    }

    bsp_display_lock(0);
    if (text && strlen(text) > 0) {
        lv_label_set_text(sd_text_label, text);
    } else {
        lv_label_set_text(sd_text_label, "SD card no valid content");
    }
    bsp_display_unlock();
}

/**
 * @brief  Initialize LVGL display interface
 * @note   Create screen background, center text label, and configure font/style
 * @return None
 */
void lvgl_display_init(void)
{
    lv_obj_t *screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
    lv_obj_set_size(screen,  BSP_LCD_H_RES,  BSP_LCD_V_RES);

    sd_text_label = lv_label_create(screen);
    if (sd_text_label == NULL) {
        ESP_LOGE(TAG, "Failed to create LVGL label");
        return;
    }

    lv_label_set_long_mode(sd_text_label, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_align(sd_text_label, LV_TEXT_ALIGN_CENTER, LV_PART_MAIN);
    lv_obj_set_style_text_color(sd_text_label, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_set_style_text_font(sd_text_label, &lv_font_montserrat_14, LV_PART_MAIN);

    lv_obj_set_size(sd_text_label, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_center(sd_text_label);

    lvgl_update_sd_text("Waiting for SD card...");
}

/**
 * @brief  Core task for SD card file reading and display
 * @note   Implemented via BSP drivers: Mount SD card -> Read file -> Display content -> Unmount safely
 * @return None
 */
static void sdcard_display_task(void)
{
    char read_buf[MAX_TEXT_LEN] = {0};
    
    // Mount SD card using BSP API
    if(bsp_sdcard_mount() != ESP_OK) {
        lvgl_update_sd_text("SD card not detected!");
        return;
    }

    // Read text file from SD card
    FILE *f = fopen(BSP_SD_MOUNT_POINT "/Test.txt", "r");
    if(f) {
        fread(read_buf, 1, MAX_TEXT_LEN-1, f);
        fclose(f);
        lvgl_update_sd_text(read_buf);
    } else {
        lvgl_update_sd_text("File read failed!");
    }

    // Unmount SD card using BSP API
    bsp_sdcard_unmount();
}

/**
 * @brief  Main function: System initialization and application startup
 * @return None
 */
void app_main(void)
{
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize display module
    bsp_display_start();
    bsp_display_backlight_on();
    vTaskDelay(pdMS_TO_TICKS(500)); 

    // Initialize LVGL UI
    bsp_display_lock(-1); 
    lvgl_display_init();
    bsp_display_unlock();   

    // Start SD card display task
    sdcard_display_task();

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}