cmake_minimum_required(VERSION 3.20)

cmake_path(GET CMAKE_CURRENT_SOURCE_DIR FILENAME PROJECT )

project(${PROJECT} LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(TARGET main)
set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/${TARGET})

add_executable(${TARGET})

set_target_properties(${TARGET}
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY
    ${OUTPUT_DIR}
)

set_target_properties(${TARGET}
    PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
)

target_sources(${TARGET}
    PRIVATE
    src/main.cpp
    # ...
)


if (EMSCRIPTEN)
    message(STATUS "Web build detected")


    set_target_properties(${TARGET}
        PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON

        OUTPUT_NAME "index"
        SUFFIX ".html"
    )

    target_link_options(${TARGET} PRIVATE
        "-sUSE_GLFW=3"                  # Needed for Raylib web builds
        "-sASYNCIFY"                    # Allows main loop to yield to the browser
        "-sASSERTIONS"
        # "--shell-file" "${CMAKE_CURRENT_SOURCE_DIR}/varicle/html/shell_custom.html" # (Optional) Custom HTML shell
        # Pre-pack your assets directory into the WebAssembly virtual filesystem
        # "--preload-file" "${CMAKE_CURRENT_SOURCE_DIR}/data.dat@/data.dat"
        "--use-port=emdawnwebgpu"
        "--closure=1"
    )

    target_compile_options(${TARGET}
        PRIVATE 
        "--use-port=emdawnwebgpu"
    )
    # target_link_libraries(${TARGET} PRIVATE emdawnwebgpu_cpp webgpu_glfw)


endif()



# target_include_directories(${TARGET} PRIVATE ...)

# add_subdirectory(...)

target_link_libraries(${TARGET} PRIVATE glfw3 )
