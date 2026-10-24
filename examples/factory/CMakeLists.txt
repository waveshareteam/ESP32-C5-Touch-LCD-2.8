cmake_minimum_required(VERSION 3.16)
# Build only components reachable from the factory APP.
set(COMPONENTS main)
set(PROJECT_VER "V1.0")
set(EXTRA_COMPONENT_DIRS "../esp-idf/components/esp32_c5_touch_lcd_2_8"
    "../esp-idf/components/pedrominatel__shtc3"
    "../esp-idf/components/waveshare__esp_lcd_touch_cst3530")
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(01_factory)
include(../esp-idf/components/idf_compat.cmake)
