
idf_component_register(
    SRCS main.c ${LV_DEMOS_SOURCES}
    INCLUDE_DIRS . ${LV_DEMO_DIR}
    PRIV_REQUIRES driver nvs_flash ball_qmi esp32_c5_touch_lcd_2_8 ball_qmi waveshare__qmi8658
)

