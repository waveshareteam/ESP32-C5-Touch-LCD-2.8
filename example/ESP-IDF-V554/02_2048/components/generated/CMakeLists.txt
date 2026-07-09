file(GLOB GENERATED_SRCS
    "*.c"
    "guider_customer_fonts/*.c"
    "guider_fonts/*.c"
    "images/*.c"
)

idf_component_register(
    SRCS ${GENERATED_SRCS}
    INCLUDE_DIRS "." "${CMAKE_CURRENT_SOURCE_DIR}/../../managed_components/lvgl__lvgl"
    REQUIRES lvgl__lvgl custom  
)