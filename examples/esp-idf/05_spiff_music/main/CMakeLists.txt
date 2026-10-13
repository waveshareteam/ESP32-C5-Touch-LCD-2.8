idf_component_register(
    SRCS "main.c"      
    INCLUDE_DIRS "." 
    REQUIRES generated custom esp32_c5_touch_lcd_2_8 waveshare__qmi8658 bsp_extra
)


spiffs_create_partition_image(storage ../spiffs FLASH_IN_PROJECT) 
