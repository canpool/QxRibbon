cmake_minimum_required(VERSION 3.5)

set(QX_VERSION_RIBBON 0.5.0)
project(QtRibbon LANGUAGES CXX)

option(BUILD_EXAMPLES "build the examples" ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
    set(QX_RIBBON_PLATFORM "x86")
else()
    set(QX_RIBBON_PLATFORM "x64")
endif()

set(QX_RIBBON_BIN_DIR "${CMAKE_BINARY_DIR}/${QX_RIBBON_PLATFORM}")
set(QX_RIBBON_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")

add_subdirectory(src)

if(BUILD_EXAMPLES)
    add_subdirectory(demo)
endif()
