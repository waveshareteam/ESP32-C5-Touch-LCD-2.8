cmake_minimum_required(VERSION 3.16)
set(COMPONENTS main)
set(EXTRA_COMPONENT_DIRS "../components/esp32_c5_touch_lcd_2_8"
    "../components/pedrominatel__shtc3" "../components/waveshare__esp_lcd_touch_cst3530")
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(02_sd_card)
include(../components/idf_compat.cmake)
