# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.12.4)

project(gui_guider)
add_compile_options(-O2)

option(CONFIG_GG_USE_G2D "Enable G2D" OFF)

find_package(PkgConfig)
pkg_check_modules(PKG_WAYLAND wayland-client wayland-cursor xkbcommon)
pkg_check_modules(PKG_LIBDRM REQUIRED libdrm)

# Wayland protocols
find_program(WAYLAND_SCANNER_EXECUTABLE NAMES wayland-scanner)
pkg_check_modules(WAYLAND_PROTOCOLS REQUIRED wayland-protocols>=1.25)
pkg_get_variable(WAYLAND_PROTOCOLS_BASE wayland-protocols pkgdatadir)

macro(wayland_generate protocol_xml_file output_dir output_name target)
    set(output_file_noext "${output_dir}/${output_name}")
    add_custom_command(OUTPUT "${output_file_noext}.h"
        COMMAND "${WAYLAND_SCANNER_EXECUTABLE}" client-header "${protocol_xml_file}" "${output_file_noext}.h"
        DEPENDS "${protocol_xml_file}"
        VERBATIM)

    add_custom_command(OUTPUT "${output_file_noext}.c"
        COMMAND "${WAYLAND_SCANNER_EXECUTABLE}" private-code "${protocol_xml_file}" "${output_file_noext}.c"
        DEPENDS "${protocol_xml_file}"
        VERBATIM)

    if(NOT EXISTS ${protocol_xml_file})
        message("Protocol XML file not found: " ${protocol_xml_file})
    else()
        set_property(TARGET ${target} APPEND PROPERTY SOURCES  "${output_file_noext}.h" "${output_file_noext}.c")
    endif()
endmacro()

set(WAYLAND_PROTOCOLS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/ports/linux/wl_protocols")
file(MAKE_DIRECTORY ${WAYLAND_PROTOCOLS_DIR})

add_custom_target(generate_xdg_shell_protocol ALL)
wayland_generate("${WAYLAND_PROTOCOLS_BASE}/stable/xdg-shell/xdg-shell.xml" "${WAYLAND_PROTOCOLS_DIR}" "wayland_xdg_shell"  generate_xdg_shell_protocol)

add_custom_target(generate_linux_dmabuf_protocol ALL)
wayland_generate("${WAYLAND_PROTOCOLS_BASE}/stable/linux-dmabuf/linux-dmabuf-v1.xml" "${WAYLAND_PROTOCOLS_DIR}" "wayland_linux_dmabuf"  generate_linux_dmabuf_protocol)

if(EXISTS ${CMAKE_SOURCE_DIR}/generated/gg_video.c)
FILE(GLOB_RECURSE SOURCES ./custom/*.c ./custom/*.cpp ./generated/*.c ports/linux/mouse_cursor_icon.c ports/linux/main.c ports/linux/video/h264_dec.cpp)
elseif(EXISTS ${CMAKE_SOURCE_DIR}/custom/real_time_edge)
FILE(GLOB_RECURSE SOURCES ./custom/*.c ./generated/*.c ports/linux/mouse_cursor_icon.c)
else()
FILE(GLOB_RECURSE SOURCES ./custom/*.c ./custom/*.cpp ./generated/*.c ports/linux/mouse_cursor_icon.c ports/linux/main.c)
endif()

add_executable (gui_guider ${SOURCES} ${WAYLAND_PROTOCOLS_DIR}/wayland_xdg_shell.c ${WAYLAND_PROTOCOLS_DIR}/wayland_linux_dmabuf.c)
if(EXISTS ${CMAKE_SOURCE_DIR}/lvgl)
add_subdirectory(lvgl)
target_include_directories(lvgl PRIVATE ${WAYLAND_PROTOCOLS_DIR})
target_include_directories(lvgl PUBLIC ${PKG_LIBDRM_INCLUDE_DIRS})
target_include_directories(gui_guider PRIVATE lvgl/src lvgl/src/font)
add_dependencies(lvgl generate_xdg_shell_protocol generate_linux_dmabuf_protocol)
endif()

target_link_libraries(gui_guider PUBLIC lvgl_demos lvgl ${PKG_WAYLAND_LIBRARIES} ${PKG_LIBDRM_LIBRARIES} -lm)
target_include_directories(gui_guider PRIVATE generated custom generated/guider_customer_fonts generated/guider_fonts generated/images)

if(EXISTS ${CMAKE_SOURCE_DIR}/generated/gg_video.c)
target_link_libraries(gui_guider PUBLIC -lopenh264)
target_include_directories(gui_guider PRIVATE ports/linux/video)
endif()

if(EXISTS ${CMAKE_SOURCE_DIR}/custom/camera_support.cpp)
pkg_check_modules(PKG_OPENCV opencv4)
target_link_libraries(gui_guider PUBLIC ${PKG_OPENCV_LIBRARIES})
target_include_directories(gui_guider PRIVATE ${PKG_OPENCV_INCLUDE_DIRS})
endif()

if(CONFIG_GG_USE_G2D)
target_link_libraries(gui_guider PUBLIC g2d)
endif()

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/gui_guider DESTINATION bin)
