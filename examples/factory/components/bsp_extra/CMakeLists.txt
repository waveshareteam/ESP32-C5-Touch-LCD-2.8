idf_component_register(SRCS "src/bsp_board_extra.cpp"
    INCLUDE_DIRS "include" REQUIRES esp32_c5_touch_lcd_2_8
    PRIV_REQUIRES esp_timer)
