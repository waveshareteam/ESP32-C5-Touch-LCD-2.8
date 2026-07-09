/*
 * SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 * @brief ESP BSP: ESP32-C5-Touch-2.8
 */

#pragma once

#include "sdkconfig.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"
#include "driver/i2s_std.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "esp_vfs_fat.h"
#include "bsp/config.h"
#include "bsp/display.h"
#include "custom_io_expander_ch32v003.h"
#include "esp_codec_dev.h"


#include "lvgl.h"
#include "esp_lv_adapter.h"

#include "qmi8658.h"
#include "pcf85063a.h"  
#include "shtc3.h"

/**************************************************************************************************
 *  BSP Capabilities
 **************************************************************************************************/

#define BSP_CAPS_DISPLAY        1
#define BSP_CAPS_TOUCH          1
#define BSP_CAPS_BUTTONS        1
#define BSP_CAPS_AUDIO          1
#define BSP_CAPS_AUDIO_SPEAKER  1
#define BSP_CAPS_AUDIO_MIC      1
#define BSP_CAPS_SDCARD         1
#define BSP_CAPS_IMU            1

/**************************************************************************************************
 * ESP-SparkBot-BSP pinout
 **************************************************************************************************/

/* I2C */
#define BSP_I2C_SCL           (GPIO_NUM_1)
#define BSP_I2C_SDA           (GPIO_NUM_0)

/* I2S */
#define BSP_I2S_MCLK          (GPIO_NUM_NC)
#define BSP_I2S_SCLK          (GPIO_NUM_24)
#define BSP_I2S_LCLK          (GPIO_NUM_25)
#define BSP_I2S_DOUT          (GPIO_NUM_26)
#define BSP_I2S_DSIN          (GPIO_NUM_27)
#define BSP_POWER_AMP_IO      (GPIO_NUM_NC)


/* Display */
#define BSP_LCD_CS              (GPIO_NUM_10)
#define BSP_LCD_PCLK            (GPIO_NUM_6)
#define BSP_LCD_DC              (GPIO_NUM_9)
#define BSP_LCD_MOSI            (GPIO_NUM_7)
#define BSP_LCD_MISO            (GPIO_NUM_NC)
#define BSP_LCD_BACKLIGHT       (GPIO_NUM_NC)
#define BSP_LCD_RST             (GPIO_NUM_NC)
#define BSP_LCD_TOUCH_RST       (GPIO_NUM_NC)
#define BSP_LCD_TOUCH_INT       (GPIO_NUM_5)

/* uSD card */
#define BSP_SD_MOSI           (GPIO_NUM_7)
#define BSP_SD_SCK            (GPIO_NUM_6)
#define BSP_SD_MISO           (GPIO_NUM_8)
#define BSP_SD_SDCS           (GPIO_NUM_23)

/* IO extion */
#define IO_LCD_TOUCH_RST     (IO_EXPANDER_PIN_NUM_0)
#define IO_LCD_RST           (IO_EXPANDER_PIN_NUM_1)
#define IO_POWER_AMP_IO      (IO_EXPANDER_PIN_NUM_3)


#define BSP_IO_EXPANDER_I2C_ADDRESS     (CUSTOM_IO_EXPANDER_I2C_CH32V003_ADDRESS)
#define LVGL_BUFFER_HEIGHT              (CONFIG_BSP_DISPLAY_LVGL_BUF_HEIGHT)

#ifdef __cplusplus
extern "C" {
#endif

/**************************************************************************************************
 *
 * I2C interface
 *
 * There are two devices connected to I2C peripheral:
 *  - QMA7981 Inertial measurement unit
 *  - OV2640 Camera module
 **************************************************************************************************/
#define BSP_I2C_NUM     I2C_NUM_0

/**
 * @brief Init I2C driver
 *
 * @return
 *      - ESP_OK                On success
 *      - ESP_ERR_INVALID_ARG   I2C parameter error
 *      - ESP_FAIL              I2C driver installation error
 *
 */
esp_err_t bsp_i2c_init(void);

/**
 * @brief Deinit I2C driver and free its resources
 *
 * @return
 *      - ESP_OK                On success
 *      - ESP_ERR_INVALID_ARG   I2C parameter error
 *
 */
esp_err_t bsp_i2c_deinit(void);

/**
 * @brief Get I2C driver handle
 *
 * @return
 *      - I2C handle
 *
 */
i2c_master_bus_handle_t bsp_i2c_get_handle(void);

/**************************************************************************************************
 *
 * I2S audio interface
 *
 * There are two devices connected to the I2S peripheral:
 *  - Codec ES8311 for output(playback) and input(recording) path
 *
 * For speaker initialization use bsp_audio_codec_speaker_init() which is inside initialize I2S with bsp_audio_init().
 * For microphone initialization use bsp_audio_codec_microphone_init() which is inside initialize I2S with bsp_audio_init().
 * After speaker or microphone initialization, use functions from esp_codec_dev for play/record audio.
 * Example audio play:
 * \code{.c}
 * esp_codec_dev_set_out_vol(spk_codec_dev, DEFAULT_VOLUME);
 * esp_codec_dev_open(spk_codec_dev, &fs);
 * esp_codec_dev_write(spk_codec_dev, wav_bytes, bytes_read_from_spiffs);
 * esp_codec_dev_close(spk_codec_dev);
 * \endcode
 **************************************************************************************************/

/**
 * @brief Init audio
 *
 * @note There is no deinit audio function. Users can free audio resources by calling i2s_del_channel()
 * @warning The type of i2s_config param is depending on IDF version.
 * @param[in]  i2s_config I2S configuration. Pass NULL to use default values (Mono, duplex, 16bit, 22050 Hz)
 * @return
 *      - ESP_OK                On success
 *      - ESP_ERR_NOT_SUPPORTED The communication mode is not supported on the current chip
 *      - ESP_ERR_INVALID_ARG   NULL pointer or invalid configuration
 *      - ESP_ERR_NOT_FOUND     No available I2S channel found
 *      - ESP_ERR_NO_MEM        No memory for storing the channel information
 *      - ESP_ERR_INVALID_STATE This channel has not initialized or already started
 */

#define BSP_I2S_NUM     I2S_NUM_0

esp_err_t bsp_audio_init(const i2s_std_config_t *i2s_config);

/**
 * @brief Get codec I2S interface (initialized in bsp_audio_init)
 *
 * @return
 *      - Pointer to codec I2S interface handle or NULL when error occurred
 */
const audio_codec_data_if_t *bsp_audio_get_codec_itf(void);

/**
 * @brief Initialize speaker codec device
 *
 * @return Pointer to codec device handle or NULL when error occurred
 */
esp_codec_dev_handle_t bsp_audio_codec_speaker_init(void);

/**
 * @brief Initialize microphone codec device
 *
 * @return Pointer to codec device handle or NULL when error occurred
 */
esp_codec_dev_handle_t bsp_audio_codec_microphone_init(void);

/**************************************************************************************************
 *
 * SPIFFS
 *
 * After mounting the SPIFFS, it can be accessed with stdio functions ie.:
 * \code{.c}
 * FILE* f = fopen(BSP_SPIFFS_MOUNT_POINT"/hello.txt", "w");
 * fprintf(f, "Hello World!\n");
 * fclose(f);
 * \endcode
 **************************************************************************************************/
#define BSP_SPIFFS_MOUNT_POINT      CONFIG_BSP_SPIFFS_MOUNT_POINT

/**
 * @brief Mount SPIFFS to virtual file system
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if esp_vfs_spiffs_register was already called
 *      - ESP_ERR_NO_MEM if memory can not be allocated
 *      - ESP_FAIL if partition can not be mounted
 *      - other error codes
 */
esp_err_t bsp_spiffs_mount(void);

/**
 * @brief Unmount SPIFFS from virtual file system
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if already unmounted
 */
esp_err_t bsp_spiffs_unmount(void);

/**************************************************************************************************
 *
 * uSD card
 *
 * After mounting the uSD card, it can be accessed with stdio functions ie.:
 * \code{.c}
 * FILE* f = fopen(BSP_MOUNT_POINT"/hello.txt", "w");
 * fprintf(f, "Hello %s!\n", bsp_sdcard->cid.name);
 * fclose(f);
 * \endcode
 **************************************************************************************************/
#define BSP_SD_MOUNT_POINT      CONFIG_BSP_SD_MOUNT_POINT

/**
 * @brief BSP SD card configuration structure
 */
typedef struct {
    const esp_vfs_fat_sdmmc_mount_config_t *mount;
    sdmmc_host_t *host;
    union {
        const sdspi_device_config_t *sdspi;
    } slot;
} bsp_sdcard_cfg_t;


/**
 * @brief Mount microSD card to virtual file system
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if esp_vfs_fat_sdmmc_mount was already called
 *      - ESP_ERR_NO_MEM if memory cannot be allocated
 *      - ESP_FAIL if partition cannot be mounted
 *      - other error codes from SDMMC or SPI drivers, SDMMC protocol, or FATFS drivers
 */
esp_err_t bsp_sdcard_mount(void);


/**
 * @brief Unmount microSD card from virtual file system
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_NOT_FOUND if the partition table does not contain FATFS partition with given label
 *      - ESP_ERR_INVALID_STATE if esp_vfs_fat_spiflash_mount was already called
 *      - ESP_ERR_NO_MEM if memory can not be allocated
 *      - ESP_FAIL if partition can not be mounted
 *      - other error codes from wear levelling library, SPI flash driver, or FATFS drivers
 */
esp_err_t bsp_sdcard_unmount(void);

/**
 * @brief Get SD card handle
 *
 * @return SD card handle
 */
sdmmc_card_t *bsp_sdcard_get_handle(void);



/**
 * @brief Get SD card SPI host config
 *
 * @param slot SD card slot
 * @param config Structure which will be filled
 */
void bsp_sdcard_get_sdspi_host(const int slot, sdmmc_host_t *config);

/**
 * @brief Get SD card SPI slot config
 *
 * @param spi_host SPI host ID
 * @param config Structure which will be filled
 */
void bsp_sdcard_sdspi_get_slot(const spi_host_device_t spi_host, sdspi_device_config_t *config);


/**
 * @brief Mount microSD card to virtual file system (SPI mode)
 *
 * @param cfg SD card configuration
 *
 * @return
 *      - ESP_OK on success
 *      - ESP_ERR_INVALID_STATE if esp_vfs_fat_sdmmc_mount was already called
 *      - ESP_ERR_NO_MEM if memory cannot be allocated
 *      - ESP_FAIL if partition cannot be mounted
 *      - other error codes from SDMMC or SPI drivers, SDMMC protocol, or FATFS drivers
 */
esp_err_t bsp_sdcard_sdspi_mount(bsp_sdcard_cfg_t *cfg);


typedef struct {
    char **list;   
    int   count;  
} generic_file_list_t;

/**
 * @brief Get list of files with specified extension from directory
 *
 * @param[in]  dir_path      Path to target directory
 * @param[in]  extension     Target file extension (e.g. ".jpg", ".wav")
 * @param[out] out           Output structure with file list and count
 *
 * @return
 *      - ESP_OK                On success
 *      - ESP_ERR_INVALID_ARG   Invalid input parameter
 *      - ESP_ERR_NOT_FOUND     No matching files found
 *      - ESP_ERR_NO_MEM        Memory allocation failed
 *      - ESP_FAIL              Failed to open directory
 */
esp_err_t get_file_list_by_ext(const char *dir_path, const char *extension, generic_file_list_t *out);

/**************************************************************************************************
 *
 * IO Expander Interface
 *
 **************************************************************************************************/
/**
 * @brief Init Custom IO expander chip CH32V003
 *
 * @note If the device was already initialized, users can also use it to get handle.
 * @note This function will be called in `bsp_display_start()` when using LCD sub-board 2 with the resolution of 480x480.
 * @note This function will be called in `bsp_audio_init()`.
 *
 * @return Pointer to device handle or NULL when error occurred
 */
esp_err_t bsp_io_expander_init(void);

/**
 * @brief Get IO expander chip handle
 *
 * @return
 *      - IO expander handle        On success
 *      - NULL                      If not initialized
 */
esp_io_expander_handle_t bsp_get_io_expander_handle(void);


/**************************************************************************************************
 *
 * Sensor Interface
 *
 **************************************************************************************************/
/**
 * @brief Init QMI8658 6-axis motion sensor (Accelerometer + Gyroscope)
 *
 * @note If the device was already initialized, users can also use it to get the device handle.
 * @note This is the BSP layer initialization interface for QMI8658 sensor.
 *
 * @return ESP_OK on success, other error codes on failure
 */
esp_err_t qmi_init(void);

/**
 * @brief Init PCF85063A real-time clock (RTC) module
 *
 * @note If the device was already initialized, users can also use it to get the device handle.
 * @note This is the BSP layer initialization interface for PCF85063A RTC.
 *
 * @return ESP_OK on success, other error codes on failure
 */
esp_err_t pcf_init(void);

/**
 * @brief Init SHTC3 temperature and humidity sensor
 *
 * @note If the device was already initialized, users can also use it to get the device handle.
 * @note This is the BSP layer initialization interface for SHTC3 temperature and humidity sensor.
 *
 * @return ESP_OK on success, other error codes on failure
 */
esp_err_t shtc3_init(void);

/**************************************************************************************************
 *
 * LCD interface
 *
 * ESP-SparkBot-BSP is shipped with 1.3inch ST7789 display controller.
 * It features 16-bit colors and 240x240 resolution.
 *
 * LVGL is used as graphics library. LVGL is NOT thread safe, therefore the user must take LVGL mutex
 * by calling bsp_display_lock() before calling any LVGL API (lv_...) and then give the mutex with
 * bsp_display_unlock().
 *
 * If you want to use the display without LVGL, see bsp/display.h API and use BSP version with 'noglib' suffix.
 **************************************************************************************************/

#if (BSP_CONFIG_NO_GRAPHIC_LIB == 0)
#define BSP_LCD_DRAW_BUFF_SIZE     (BSP_LCD_H_RES * CONFIG_BSP_LCD_RGB_BOUNCE_BUFFER_HEIGHT)
#define BSP_LCD_DRAW_BUFF_DOUBLE   (1)


/**
 * @brief BSP display configuration structure
 */
typedef struct {
    esp_lv_adapter_config_t          lv_adapter_cfg;
    esp_lv_adapter_rotation_t        rotation;
    esp_lv_adapter_tear_avoid_mode_t tear_avoid_mode;
    struct {
        unsigned int swap_xy;  /*!< Swap X and Y after read coordinates */
        unsigned int mirror_x; /*!< Mirror X after read coordinates */
        unsigned int mirror_y; /*!< Mirror Y after read coordinates */
    } touch_flags;
} bsp_display_cfg_t;




/**
 * @brief Initialize display (SPI 屏 ST7789)
 *
 * This function initializes SPI, display controller and starts LVGL handling task.
 *
 * @return Pointer to LVGL display or NULL when error occurred
 */
lv_display_t *bsp_display_start(void);

/**
 * @brief Initialize display (SPI 屏 ST7789) with custom config
 *
 * This function initializes SPI, display controller and starts LVGL handling task.
 * LCD backlight must be enabled separately by calling bsp_display_brightness_set()
 *
 * @param cfg display configuration
 *
 * @return Pointer to LVGL display or NULL when error occurred
 */
lv_display_t *bsp_display_start_with_config(bsp_display_cfg_t *cfg);

/**
 * @brief Get pointer to display device 
 *
 * @note The LVGL display device is initialized in bsp_display_start() function.
 *
 * @return Pointer to LVGL display device or NULL when not initialized
 */
lv_display_t *bsp_display_get_disp_dev(void);

/**
 * @brief Get pointer to input device (touch, buttons, ...)
 *
 * @note The LVGL input device is initialized in bsp_display_start() function.
 *
 * @return Pointer to LVGL input device or NULL when not initialized
 */
lv_indev_t *bsp_display_get_input_dev(void);

/**
 * @brief Take LVGL mutex
 *
 * @param timeout_ms Timeout in [ms]. 0 will block indefinitely.
 * @return true  Mutex was taken
 * @return false Mutex was NOT taken
 */
esp_err_t bsp_display_lock(uint32_t timeout_ms);

/**
 * @brief Give LVGL mutex
 *
 */
void bsp_display_unlock(void);

#endif // BSP_CONFIG_NO_GRAPHIC_LIB == 0

#ifdef __cplusplus
}
#endif