file(GLOB_RECURSE MAIN_SRCS ./*.cpp ./*.c)
set(EMBEDDED_FILES "")

if (CONFIG_EXAMPLE_COZE_AGENT_PRIVATE_KEY_FILE_PATH)
    list(APPEND EMBEDDED_FILES ${CONFIG_EXAMPLE_COZE_AGENT_PRIVATE_KEY_FILE_PATH})
    message(STATUS "Coze Private Key Path: ${CONFIG_EXAMPLE_COZE_AGENT_PRIVATE_KEY_FILE_PATH}")
endif()

idf_component_register(
    SRCS ${MAIN_SRCS}
    INCLUDE_DIRS "."
    REQUIRES esp32_c5_touch_lcd_2_8 bsp_extra brookesia_core DrawingBoard Games AIChats MicTest Qmi8658Ball Musicplayer app_settings Photos Weather Sensor system_manage_service esp_wifi esp_event esp_netif nvs_flash lwip
)

target_compile_options(${COMPONENT_LIB} PUBLIC -Wno-missing-field-initializers)


function(get_component_library component_name output_var)
    # Get the exact component name
    idf_build_get_property(build_components BUILD_COMPONENTS)
    set(TARGET_COMPONENT "")
    foreach(COMPONENT ${build_components})
        if(COMPONENT MATCHES "${component_name}" OR COMPONENT MATCHES "espressif__${component_name}")
            set(TARGET_COMPONENT ${COMPONENT})
            break()
        endif()
    endforeach()

    # Get the component library
    if(TARGET_COMPONENT STREQUAL "")
        message(FATAL_ERROR "Component '${component_name}' not found.")
    else()
        idf_component_get_property(COMPONENT_LIB ${TARGET_COMPONENT} COMPONENT_LIB)
        set(${output_var} ${COMPONENT_LIB} PARENT_SCOPE)
    endif()
endfunction()

spiffs_create_partition_image(storage ../spiffs FLASH_IN_PROJECT) 
