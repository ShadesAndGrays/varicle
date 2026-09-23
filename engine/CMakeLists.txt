cmake_minimum_required(VERSION 3.20)

set(TARGET VaricleEngine)

project(${TARGET})

add_library(${TARGET})
add_library(Varicle::Engine ALIAS ${TARGET})

file(GLOB_RECURSE ENGINE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

file(GLOB IMGUI_SOURCES "vendor/imgui/*.cpp")

target_sources(${TARGET}
    PRIVATE
    ${ENGINE_SRC}
    ${IMGUI_SOURCES}
)

target_include_directories(${TARGET} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/vendor/imgui>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/vendor/rlImGui>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/vendor/entt>
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/vendor/json>

)

target_link_libraries(${TARGET} PUBLIC
    box2d
    AssetPacker::assetpacker
)
