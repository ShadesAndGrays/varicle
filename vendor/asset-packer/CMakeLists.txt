cmake_minimum_required(VERSION 4.0.0)
project(assetpacker CXX)

# Guard against duplicate inclusions
if(TARGET ${PROJECT_NAME}_lib)
    return()
endif()

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

# ------------------------------------------------------------------------------
# Library Definition
# ------------------------------------------------------------------------------

# Create an ALIAS target so parent targets can link using namespace notation:
# e.g., target_link_libraries(my_app PRIVATE AssetPacker::assetpacker)
add_library(${PROJECT_NAME}_lib STATIC src/lib.cpp)
add_library(AssetPacker::assetpacker ALIAS ${PROJECT_NAME}_lib)

# Use BUILD_INTERFACE so includes work seamlessly inside this tree
# and when linked by a parent project without manually copying headers around.
target_include_directories(${PROJECT_NAME}_lib
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src/include>
        $<INSTALL_INTERFACE:include>
)

# target_sources(${PROJECT_NAME}_lib PUBLIC
#     FILE_SET HEADERS
#     BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src/include
#     FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/include/asset-packer.hpp
# )
#
# Apply Emscripten-specific flags if building for Web
if(EMSCRIPTEN)
    target_compile_options(${PROJECT_NAME}_lib PRIVATE -O2)
endif()

set_target_properties(${PROJECT_NAME}_lib PROPERTIES OUTPUT_NAME "assetpacker")

# ------------------------------------------------------------------------------
# Executable
# ------------------------------------------------------------------------------

if(NOT EMSCRIPTEN)
    add_executable(${PROJECT_NAME} src/main.cpp)
    target_link_libraries(assetpacker PRIVATE assetpacker_lib)
    add_executable(AssetPacker::packer ALIAS assetpacker)
endif()

# ------------------------------------------------------------------------------
# Tests (Optional for Parent)
# ------------------------------------------------------------------------------

# Only build standalones/tests if this project is the top-level project
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


    add_custom_command(
        TARGET assetpacker POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/asset"
                "$<TARGET_FILE_DIR:assetpacker>/asset"
        COMMENT "Copying assets to build directory..."
    )

    # Test Executable
    enable_testing()
    add_executable(assetpacker_test src/test.cpp)
    target_link_libraries(assetpacker_test PRIVATE assetpacker_lib)

    add_custom_command(
        TARGET assetpacker_test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/asset"
                "$<TARGET_FILE_DIR:assetpacker_test>/stuff"
        COMMENT "Copying assets for test target..."
    )

    add_test(NAME RunTests COMMAND assetpacker_test)
endif()
