idf_component_register(
    SRCS "esp32_c5_touch_lcd_2.8.c" ${SRC_VER}
    INCLUDE_DIRS "include" "include/bsp"
    PRIV_INCLUDE_DIRS "priv_include"
     REQUIRES fatfs esp_driver_i2c  esp_driver_gpio esp_lcd waveshare__qmi8658 waveshare__pcf85063a pedrominatel__shtc3 waveshare__esp_lcd_touch_cst3530 
    PRIV_REQUIRES esp_timer spiffs esp_psram usb
)

