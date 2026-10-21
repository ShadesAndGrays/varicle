cmake_minimum_required(VERSION 3.29) # module support in version 3.28

set (TARGET "VaricleVulkan")

add_library(${TARGET})
add_library(Varicle::Vulkan ALIAS ${TARGET})

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
# set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -static")

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

find_package(glfw3 REQUIRED)
# find_package(tinyobjloader REQUIRED)
# find_package(tinygltf REQUIRED)

# find_package(glm REQUIRED)
find_package(Vulkan 1.4.335 REQUIRED)
find_package(KTX REQUIRED)

add_subdirectory(${CMAKE_SOURCE_DIR}/vendor/slang
    ${CMAKE_BINARY_DIR}/vendor/slang)


set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_WAYLAND OFF CACHE BOOL "" FORCE)

target_compile_definitions(${TARGET}
    PUBLIC
    VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
    GLM_FORCE_DEPTH_ZERO_TO_ONE

    PRIVATE
    GLFW_INCLUDE_VULKAN
    VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
    VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS=1
    GLM_ENABLE_EXPERIMENTAL
    GLM_FORCE_DEPTH_ZERO_TO_ONE
    TINYOBJLOADER_IMPLEMENTATION
    STB_IMAGE_IMPLEMENTATION
  )

target_link_libraries (${TARGET} PRIVATE Vulkan::Vulkan glfw slang Varicle::Common)

# find_package(stb REQUIRED)
# set(STB_INCLUDEDIR ${stb_INCLUDE_DIRS})

# find_program(SLANGC_EXECUTABLE slangc HINTS $ENV{VULKAN_SDK}/bin REQUIRED)

# set(TEMPLATE_DIR ${CMAKE_BINARY_DIR}/${TARGET})



# add_subdirectory(../../../vendor/glfw ${CMAKE_CURRENT_BINARY_DIR}/vendor/glfw )

# add_executable (${TARGET})

target_sources(${TARGET}
    PRIVATE
    # test.cpp
    lib.cpp
    core/init.cpp
    core/device.cpp
    core/validation.cpp
    core/swap-chain.cpp
    graphics/image.cpp
    graphics/buffer.cpp
    graphics/pipeline.cpp
    graphics/resource.cpp
    graphics/resource-manager.cpp
    util/slang-compiler.cpp
)

target_include_directories(${TARGET}
    PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${SLANG_ROOT}/include
)

message(STATUS root ${SLANG_ROOT})


add_custom_command(TARGET ${TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${SLANG_RUNTIME_DLLS} $<TARGET_FILE_DIR:${TARGET}>
)
#
# set_target_properties (${TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEMPLATE_DIR})



# target_include_directories (${TARGET} PRIVATE ${STB_INCLUDEDIR})

# Define VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS to treat VK_ERROR_OUT_OF_DATE_KHR as a success code
# We'll handle this personally

# Slang shader
# set (SHADER_TARGET ${TARGET}Shader)
# set (SHADERS_DIR ${TEMPLATE_DIR}/shaders)
# set (SHADER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/shaders/shader.slang")
# set (ENTRY_POINTS -entry vertMain -entry fragMain)
# add_custom_command(OUTPUT ${SHADERS_DIR} COMMAND ${CMAKE_COMMAND} -E make_directory ${SHADERS_DIR})
# add_custom_command(
#   OUTPUT  ${SHADERS_DIR}/slang.spv
#   COMMAND ${SLANGC_EXECUTABLE} ${SHADER_SOURCES} -target spirv -profile spirv_1_4 -emit-spirv-directly -fvk-use-entrypoint-name ${ENTRY_POINTS} -o ${TEMPLATE_DIR}/shaders/slang.spv
#   WORKING_DIRECTORY ${SHADERS_DIR}
#   DEPENDS ${SHADERS_DIR} ${SHADER_SOURCES}
#   COMMENT "Compiling Slang Shaders"
#   VERBATIM
# )
# add_custom_command(
#   COMMAND ./compile.sh
#   WORKING_DIRECTORY ${SHADER_SOURCES}
#   DEPENDS ${SHADER_SOURCES}
#   COMMENT "Compiling Slang Shaders"
#   VERBATIM
# )
# add_custom_target(${SHADER_TARGET} DEPENDS ${SHADERS_DIR}/slang.spv)
# add_dependencies(${TARGET} ${SHADER_TARGET})


# Assets
# set (ASSET_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets/")
# file (COPY ${ASSET_DIR}/viking_room.obj DESTINATION ${TEMPLATE_DIR}/models)
# file (COPY ${ASSET_DIR}/viking_room.png DESTINATION ${TEMPLATE_DIR}/textures)
