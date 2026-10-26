idf_component_register(SRCS "Qmi8658Ball.cpp" "assets/img_app_qmi8658ball.c"
    INCLUDE_DIRS "." REQUIRES brookesia_core app_support esp32_c5_touch_lcd_2_8 esp_timer)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-missing-field-initializers)
