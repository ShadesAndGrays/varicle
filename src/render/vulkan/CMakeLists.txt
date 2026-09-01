cmake_minimum_required(VERSION 3.29) # module support in version 3.28

set (TEMPLATE_NAME "VulkanRenderer")

project (${TEMPLATE_NAME})

set(CMAKE_EXPORT_COMPILE_COMMANDS on)

# target_include_directories(${LIB_TARGET} PRIVATE src)

# Add option to enable/disable C++ 20 module
# option(ENABLE_CPP20_MODULE "Enable C++ 20 module support for Vulkan" OFF)

# Enable C++ module dependency scanning only if C++ 20 module is enabled
# if(ENABLE_CPP20_MODULE)
#   set(CMAKE_CXX_SCAN_FOR_MODULES ON)
# endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(Vulkan 1.4.335 REQUIRED)
find_package(tinyobjloader REQUIRED)
find_package(tinygltf REQUIRED)
find_package(KTX REQUIRED)

add_library(VaricleVulkan INTERFACE)
add_library(Varicle::Vulkan ALIAS VaricleVulkan)
target_link_libraries(VaricleVulkan INTERFACE Vulkan::Vulkan)
target_compile_definitions(VaricleVulkan
    INTERFACE
    VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1
    VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
  )

find_package(stb REQUIRED)
set(STB_INCLUDEDIR ${stb_INCLUDE_DIRS})

find_program(SLANGC_EXECUTABLE slangc HINTS $ENV{VULKAN_SDK}/bin REQUIRED)

set(TEMPLATE_DIR ${CMAKE_BINARY_DIR}/${TEMPLATE_NAME})

add_executable (${TEMPLATE_NAME})

target_sources(${TEMPLATE_NAME}
    PRIVATE
    src/test.cpp
    src/vulkan-renderer.cpp
    src/core/init.cpp
    src/core/device.cpp
    src/core/validation.cpp
    src/core/swap-chain.cpp
    src/graphics/buffer.cpp
    src/graphics/pipeline.cpp
    src/graphics/image.cpp
)

target_include_directories(${TEMPLATE_NAME}
    PUBLIC
    src
    ../common
)

set_target_properties (${TEMPLATE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${TEMPLATE_DIR})
set_target_properties (${TEMPLATE_NAME} PROPERTIES CXX_STANDARD 23)
target_link_libraries (${TEMPLATE_NAME} Varicle::Vulkan glfw)
target_include_directories (${TEMPLATE_NAME} PRIVATE ${STB_INCLUDEDIR})

# Define VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS to treat VK_ERROR_OUT_OF_DATE_KHR as a success code
# We'll handle this personally
target_compile_definitions(${TEMPLATE_NAME} PRIVATE "VULKAN_HPP_HANDLE_ERROR_OUT_OF_DATE_AS_SUCCESS" )

# Slang shader
# set (SHADER_TARGET ${TEMPLATE_NAME}Shader)
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
# add_custom_target(${SHADER_TARGET} DEPENDS ${SHADERS_DIR}/slang.spv)
# add_dependencies(${TEMPLATE_NAME} ${SHADER_TARGET})

# Assets
# set (ASSET_DIR "${CMAKE_CURRENT_SOURCE_DIR}/assets/")
# file (COPY ${ASSET_DIR}/viking_room.obj DESTINATION ${TEMPLATE_DIR}/models)
# file (COPY ${ASSET_DIR}/viking_room.png DESTINATION ${TEMPLATE_DIR}/textures)
