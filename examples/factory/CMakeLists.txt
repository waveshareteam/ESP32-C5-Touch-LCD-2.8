cmake_minimum_required(VERSION 3.16)
# Build only components reachable from the factory APP.
set(COMPONENTS main)
set(PROJECT_VER "V1.0")
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(01_factory)
include(components/idf_compat.cmake)
