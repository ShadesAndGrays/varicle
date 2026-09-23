cmake_minimum_required(VERSION 3.20)

set(TARGET VaricleCommon)

project(${TARGET})

add_library(${TARGET})
add_library(Varicle::Common ALIAS ${TARGET})

set_target_properties(${TARGET}
    PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
)

target_sources(${TARGET}
    PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common.cpp
)

target_include_directories(${TARGET}
    PRIVATE
    ${CMAKE_SOURCE_DIR}
)
