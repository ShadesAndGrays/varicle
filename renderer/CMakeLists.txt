cmake_minimum_required(VERSION 3.20)

set(TARGET VaricleRenderer)

project(${TARGET})

add_library(${TARGET} INTERFACE)
add_library(Varicle::Renderer ALIAS ${TARGET})

add_subdirectory(common renderer/common)
add_subdirectory(opengl renderer/opengl)
# add_subdirectory(src/render/vulkan)
# add_subdirectory(src/render/webgpu)

target_link_libraries(${TARGET}
    INTERFACE
    Varicle::Common
    Varicle::OpenGL
)

# target_include_directories(${TARGET}
#    INTERFACE
#    ${CMAKE_SOURCE_DIR}
# )
