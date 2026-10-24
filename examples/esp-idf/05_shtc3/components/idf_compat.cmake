# Product-local compatibility for the retained 1.x Waveshare driver APIs.
# ESP-IDF 6 no longer exports peripheral headers through the umbrella driver
# component. Keep downloaded components immutable and add their real dependency
# at the product layer. Revisit when those managed releases declare it themselves.
if(IDF_VERSION_MAJOR GREATER_EQUAL 6)
    cmake_policy(PUSH)
    cmake_policy(SET CMP0079 NEW)
    foreach(component waveshare__custom_io_expander_ch32v003 waveshare__qmi8658 waveshare__pcf85063a)
        if(TARGET __idf_${component})
            target_link_libraries(__idf_${component} PUBLIC idf::esp_driver_i2c)
        endif()
    endforeach()
    if(TARGET __idf_waveshare__qmi8658)
        target_include_directories(__idf_waveshare__qmi8658 BEFORE PUBLIC
            "${CMAKE_CURRENT_LIST_DIR}/compat")
    endif()
    if(TARGET __idf_espressif__esp-boost AND
       CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND
       CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 15)
        # Boost 0.3.x's generated Proto headers use the older variadic syntax.
        # GCC 15 deprecates that valid syntax; retain the warning, not -Werror.
        target_compile_options(__idf_espressif__esp-boost PUBLIC
            "$<$<COMPILE_LANGUAGE:CXX>:-Wno-error=deprecated-variadic-comma-omission>")
    endif()
    cmake_policy(POP)
endif()
