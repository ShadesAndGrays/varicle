cmake_minimum_required(VERSION 3.20)

set(TARGET VaricleOpenGL)

project(${TARGET})

# set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(${TARGET})
add_library(Varicle::OpenGL ALIAS ${TARGET})

set_target_properties(${TARGET}
    PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
)


file(GLOB SRC src/*.cpp)

target_sources(${TARGET}
    PRIVATE
    ${CMAKE_SOURCE_DIR}/vendor/glad/src/glad.c
    ${SRC}
)

target_compile_definitions(${TARGET}
    PRIVATE
    STB_IMAGE_IMPLEMENTATION
    GLM_ENABLE_EXPERIMENTAL
)

target_include_directories(${TARGET}
    PRIVATE
    ${CMAKE_SOURCE_DIR}/vendor/glad/include 
    ${CMAKE_SOURCE_DIR}/vendor/stb 
    ${CMAKE_SOURCE_DIR}
)


target_link_libraries(${TARGET} PRIVATE glfw3 Varicle::Common)


# if (EMSCRIPTEN)
#     message(STATUS "Web build detected")
#     set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/web)
#
#
#     set_target_properties(${TARGET}
#         PROPERTIES
#         CXX_STANDARD 23
#         CXX_STANDARD_REQUIRED ON
#
#         OUTPUT_NAME "index"
#         SUFFIX ".html"
#     )
#
#     target_link_options(${TARGET} PRIVATE
#         # "-sUSE_GLFW=3"                  # Needed for Raylib web builds
#         # "-sASYNCIFY"                    # Allows main loop to yield to the browser
#         # "--shell-file" "${CMAKE_CURRENT_SOURCE_DIR}/varicle/html/shell_custom.html" # (Optional) Custom HTML shell
#         # Pre-pack your assets directory into the WebAssembly virtual filesystem
#         # "--preload-file" "${CMAKE_CURRENT_SOURCE_DIR}/data.dat@/data.dat"
#     )
# endif()
