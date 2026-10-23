idf_component_register(
    SRCS "esp32_c5_touch_lcd_2.8.c" "bsp_board.c"
    INCLUDE_DIRS "include" "include/bsp"
    PRIV_INCLUDE_DIRS "priv_include"
    REQUIRES fatfs esp_driver_i2c esp_driver_gpio esp_driver_i2s esp_driver_spi esp_lcd
             waveshare__qmi8658 waveshare__pcf85063a pedrominatel__shtc3
             waveshare__esp_lcd_touch_cst3530 esp_lvgl_adapter esp_codec_dev
             esp_wifi esp_netif
    PRIV_REQUIRES bsp_extra esp_timer nvs_flash esp_event sdmmc
)
