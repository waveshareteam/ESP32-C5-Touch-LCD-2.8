file(GLOB_RECURSE MAIN_SRCS ./*.cpp ./*.c)
set(EMBEDDED_FILES "")

if (CONFIG_EXAMPLE_COZE_AGENT_PRIVATE_KEY_FILE_PATH)
    list(APPEND EMBEDDED_FILES ${CONFIG_EXAMPLE_COZE_AGENT_PRIVATE_KEY_FILE_PATH})
    message(STATUS "Coze Private Key Path: ${CONFIG_EXAMPLE_COZE_AGENT_PRIVATE_KEY_FILE_PATH}")
endif()

idf_component_register(
    SRCS ${MAIN_SRCS}
    INCLUDE_DIRS "."
    REQUIRES esp32_c5_touch_lcd_2_8 brookesia_core DrawingBoard )

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



