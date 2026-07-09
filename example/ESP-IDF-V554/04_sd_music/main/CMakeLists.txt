idf_component_register(
    SRCS "main.c"      
    INCLUDE_DIRS "." 
    REQUIRES esp32_c5_touch_lcd_2_8 shtc3 nvs_flash bsp_extra waveshare__qmi8658
)

