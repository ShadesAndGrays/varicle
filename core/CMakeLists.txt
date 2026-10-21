cmake_minimum_required(VERSION 3.20)

set(TARGET VaricleCore)

add_library(${TARGET} INTERFACE)
add_library(Varicle::Core ALIAS ${TARGET})

file(GLOB SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)

target_sources(${TARGET} PRIVATE ${SRC})

target_include_directories(${TARGET} PUBLIC $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>)

target_compile_definitions(${TARGET} INTERFACE VARICLE_MODULE_CORE)
