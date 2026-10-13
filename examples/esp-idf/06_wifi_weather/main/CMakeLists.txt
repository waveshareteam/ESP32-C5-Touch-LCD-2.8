idf_component_register(
    SRCS "main.c"      
    INCLUDE_DIRS "." 
    REQUIRES generated custom esp32_c5_touch_lcd_2_8 nvs_flash waveshare__qmi8658 
)

