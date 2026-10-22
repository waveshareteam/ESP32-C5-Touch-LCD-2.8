idf_component_register(
    SRCS "custom.c"
    INCLUDE_DIRS "." "${CMAKE_CURRENT_SOURCE_DIR}/../../managed_components/lvgl__lvgl"
    REQUIRES lvgl__lvgl generated
)