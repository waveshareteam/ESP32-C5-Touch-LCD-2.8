/********************************************************************************************************
 * @file    main.c
 * @author  Javen
 * @company Waveshare
 * @date    2026-03-25
 * @brief   ESP32-C5-Touch-LCD-2.8 | SD Card MP3 Player Demo
 *
 * @note    This demo implements automatic MP3 playback from SD card using official BSP drivers.
 *          1. Scans MP3 files in the "/sdcard/music" directory
 *          2. Initializes audio codec and plays the first MP3 track automatically
 *          3. Supports FAT32 SD card (max 64GB)
 ********************************************************************************************************/
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "esp_err.h"
#include "driver/gpio.h"
#include "lvgl.h"
#include "bsp/esp-bsp.h"
#include "bsp/display.h"
#include "bsp_board_extra.h"

static const char *TAG = "MP3_PLAYER";

/* File list structure to store MP3 file paths */
static generic_file_list_t MP3_files = {0};
static uint16_t file_count = 0;

/**
 * @brief  Scan and list all MP3 files in /sdcard/music directory
 * @note   Uses BSP file system helper function
 * @return None
 */
static void mp3_file_scan(void)
{
    // Scan MP3 files from SD card music folder
    esp_err_t err = get_file_list_by_ext("/sdcard/music", ".mp3", &MP3_files);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "Found %d MP3 file(s)", MP3_files.count);
        // Print MP3 file list to console
        for (int i = 0; i < MP3_files.count; i++) {
            ESP_LOGI(TAG, "MP3 [%d]: %s", i, MP3_files.list[i]);
        }
    } else {
        ESP_LOGE(TAG, "No MP3 files found in /sdcard/music");
        MP3_files.count = 0;
    }

    file_count = MP3_files.count;
}

/**
 * @brief  Initialize LCD CS pin to avoid SPI bus conflict
 * @note   Set CS high to disable LCD during SD card operation
 * @return None
 */
static void lcd_cs_gpio_init(void)
{
    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << BSP_LCD_CS),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE,
    };
    gpio_config(&io_conf);
    gpio_set_level(BSP_LCD_CS, 1);
}

/**
 * @brief  Auto play the first MP3 file in the list
 * @return None
 */
static void mp3_auto_play(void)
{
    // Safety check: No MP3 files
    if (file_count == 0 || MP3_files.list == NULL) {
        ESP_LOGE(TAG, "Playback failed: No valid MP3 files");
        return;
    }

    // Initialize audio driver
    Audio_Play_Init();
    // Set maximum volume
    Volume_Adjustment(99);

    // Generate audio playback URI
    char uri[128] = {0};
    snprintf(uri, sizeof(uri), "file://%s", MP3_files.list[0] + strlen("/"));
    ESP_LOGI(TAG, "Start playing: %s", uri);

    // Start MP3 playback
    Audio_Play_Music(uri);
}

void app_main(void)
{
    // 1. Initialize NVS (Required for ESP32 system)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // 2. Initialize LCD CS pin to prevent SPI conflict
    lcd_cs_gpio_init();

    // 3. Initialize board IO expander
    bsp_io_expander_init();
    vTaskDelay(pdMS_TO_TICKS(100));

    // 4. Mount SD card (BSP API)
    ret = bsp_sdcard_mount();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "SD card mount failed!");
        while (1) vTaskDelay(pdMS_TO_TICKS(1000));
    }

    // 5. Scan MP3 files
    mp3_file_scan();

    // 6. Auto play first MP3
    mp3_auto_play();

    // Main loop
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}