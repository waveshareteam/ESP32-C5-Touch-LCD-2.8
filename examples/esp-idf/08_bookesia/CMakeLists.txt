cmake_minimum_required(VERSION 3.5)

list(APPEND EXTRA_COMPONENT_DIRS "${CMAKE_CURRENT_LIST_DIR}/../components")
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(01_factory)
include("${CMAKE_CURRENT_LIST_DIR}/../components/idf_compat.cmake")
