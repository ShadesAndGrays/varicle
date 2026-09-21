cmake_minimum_required(VERSION 3.20)

cmake_path(GET CMAKE_CURRENT_SOURCE_DIR FILENAME PROJECT_DIR )

project(${PROJECT_DIR} )

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(TARGET main)
# set(OUTPUT_DIR ${CMAKE_BINARY_DIR})

add_executable(${TARGET})

set_target_properties(${TARGET}
    PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
)

target_sources(${TARGET}
    PRIVATE
    vendor/glad/src/glad.c
    src/main.cpp
    src/shader.cpp
    src/transform.cpp
    # ...
)

target_compile_definitions(${TARGET}
    PRIVATE
    STB_IMAGE_IMPLEMENTATION
    GLM_ENABLE_EXPERIMENTAL
)


if (EMSCRIPTEN)
    message(STATUS "Web build detected")
    set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/web)


    set_target_properties(${TARGET}
        PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON

        OUTPUT_NAME "index"
        SUFFIX ".html"
    )

    target_link_options(${TARGET} PRIVATE
        # "-sUSE_GLFW=3"                  # Needed for Raylib web builds
        # "-sASYNCIFY"                    # Allows main loop to yield to the browser
        # "--shell-file" "${CMAKE_CURRENT_SOURCE_DIR}/varicle/html/shell_custom.html" # (Optional) Custom HTML shell
        # Pre-pack your assets directory into the WebAssembly virtual filesystem
        # "--preload-file" "${CMAKE_CURRENT_SOURCE_DIR}/data.dat@/data.dat"
    )
endif()


target_include_directories(${TARGET} PRIVATE
    vendor/glad/include 
    vendor/stb 
)


# target_sources(${TARGET}
#     PUBLIC
# )
# add_subdirectory(...)

target_link_libraries(${TARGET} PRIVATE glfw3)

