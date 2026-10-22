set(PROJ_SRC ${CMAKE_CURRENT_LIST_DIR})

file(GLOB_RECURSE PROJ_SRCS_C ${PROJ_SRC}/*.c)
file(GLOB_RECURSE PROJ_SRCS_CPP ${PROJ_SRC}/*.cpp)

idf_component_register(
    SRCS  ${PROJ_SRCS_C} ${PROJ_SRCS_CPP}
    INCLUDE_DIRS
        ${PROJ_SRC}
        ${PROJ_SRC}/custom
        ${PROJ_SRC}/generated
        ${PROJ_SRC}/music_init
        ${PROJ_SRC}/assets
    REQUIRES
        esp_lvgl_adapter
        esp32_c5_touch_lcd_2_8
        bsp_extra
)

set_source_files_properties(
    ${PROJ_SRCS_CPP}
    PROPERTIES
        COMPILE_FLAGS "-Wno-missing-field-initializers"
)