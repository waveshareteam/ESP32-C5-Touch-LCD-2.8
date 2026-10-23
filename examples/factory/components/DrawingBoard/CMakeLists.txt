idf_component_register(SRCS "DrawingBoard.cpp" "assets/image_draw.c"
    INCLUDE_DIRS "." REQUIRES brookesia_core lvgl esp32_c5_touch_lcd_2_8)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-missing-field-initializers)
