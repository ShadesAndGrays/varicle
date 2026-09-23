cmake_minimum_required(VERSION 3.25)

project(Varicle LANGUAGES CXX)

option(BUILD_EXAMPLES "Build example applications" OFF)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_SCAN_FOR_MODULES OFF)

# ------------------------------------------------------------------------------
# 1. Platform & Dist Paths
# ------------------------------------------------------------------------------

# if(BUILD_FOR_WEB OR EMSCRIPTEN)
#     set(DIST_PLATFORM "web")
# else()
#     set(DIST_PLATFORM "desktop")
# endif()

# Automatically route 'cmake --install' into 'dist/desktop' or 'dist/web'
# if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
#     set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}/dist/${DIST_PLATFORM}" CACHE PATH "Dist directory" FORCE)
# endif()

# include(GNUInstallDirs)

# ------------------------------------------------------------------------------
# 2. Vendored Dependencies (Internal)
# ------------------------------------------------------------------------------

add_subdirectory(vendor/asset-packer)
add_subdirectory(vendor/box2d)

file(GLOB IMGUI_SOURCES "vendor/imgui/*.cpp")
file(GLOB IMGUI_HEADERS "vendor/imgui/*.h")

# ------------------------------------------------------------------------------
# 3. Main Framework Target (Single Monolithic Library)
# ------------------------------------------------------------------------------
file(GLOB_RECURSE ENGINE_SRC "src/engine/*.cpp")
set(TARGET Varicle)

add_library(${TARGET} INTERFACE)
add_library(Varicle::Varicle ALIAS ${TARGET})

# modules
add_subdirectory(engine)
add_subdirectory(renderer)

# target_sources(${PROJECT_NAME}_lib
#     PRIVATE
    # ${ENGINE_SRC}
    # ${IMGUI_SOURCES}
# )

set_target_properties(${TARGET} PROPERTIES OUTPUT_NAME "varicle")

target_include_directories(${TARGET} INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

target_link_libraries(${TARGET} INTERFACE
    Varicle::Renderer
    Varicle::Engine
)



# ------------------------------------------------------------------------------
# 4. Clean Export & Install
# ------------------------------------------------------------------------------
# Install engine library + raylib
# install(TARGETS ${PROJECT_NAME}_lib assetpacker_lib
#     EXPORT ${PROJECT_NAME}Targets
#     ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#     RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#     PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
# )

# Copy header files straight into dist/include for consumer projects
# install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/src/" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
# install(FILES ${IMGUI_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/imgui )
# install(FILES ${RLIMGUI_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/rlImGui )
# install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/vendor/entt/entt" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
# install(FILES "${CMAKE_CURRENT_SOURCE_DIR}/vendor/json/json.hpp" DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/json )

# Standard CMake package export
# install(EXPORT ${PROJECT_NAME}Targets
#     FILE ${PROJECT_NAME}Targets.cmake
#     NAMESPACE ${PROJECT_NAME}::
#     DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME}
# )


# ------------------------------------------------------------------------------
# 5. Standalone Sandbox App / Executable
# ------------------------------------------------------------------------------
# file(GLOB_RECURSE APP_SRC "src/*.cpp")
# add_executable(${PROJECT_NAME} ${APP_SRC})
#
# target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_lib)
#
# if(EMSCRIPTEN)
#     set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "index" SUFFIX ".html")
#     target_link_options(${PROJECT_NAME} PRIVATE
#         "-sUSE_GLFW=3"
#         "-sASYNCIFY"
#         "--shell-file" "${CMAKE_CURRENT_SOURCE_DIR}/html/shell_custom.html"
#     )
# endif()

# #------------------------------------------------------------------------------------------
# # Make Assets
# #------------------------------------------------------------------------------------------
#
# set(BUILD_ASSET_DIR "${CMAKE_CURRENT_BINARY_DIR}/assets")
# set(PACKED_DATA_FILE "${CMAKE_CURRENT_BINARY_DIR}/data.dat")
# #
# add_custom_target( pack-asset
#     COMMAND ${CMAKE_COMMAND} -E copy_directory
#             "${CMAKE_CURRENT_SOURCE_DIR}/assets"
#             "${BUILD_ASSET_DIR}"
#
#
#     # 2. Pass relative paths to assetpacker!
#     # Because WORKING_DIRECTORY is ${CMAKE_CURRENT_BINARY_DIR},
#     # 'assets' resolves to './assets' and 'data.dat' resolves to './data.dat'
#
#     # Set execution context so relative paths inside assetpacker work
#     WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
#
#     COMMAND AssetPacker::packer "assets/"
#
#     DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/assets" AssetPacker::packer
#     COMMENT "Copying assets folder and packing via relative paths..."
#     VERBATIM
#
# )


# # ------------------------------------------------------------------------------
# # WebAssembly / Emscripten Configurations
# # ------------------------------------------------------------------------------

# if(EMSCRIPTEN)
#     # Output an HTML page instead of a raw binary
#     set_target_properties(${PROJECT_NAME} PROPERTIES
#         OUTPUT_NAME "index"
#         SUFFIX ".html"
#     )
#
#     # Pass Emscripten linker flags
#     target_link_options(${PROJECT_NAME} PRIVATE
#         "-sUSE_GLFW=3"                  # Needed for Raylib web builds
#         "-sASYNCIFY"                    # Allows main loop to yield to the browser
#         "--shell-file" "${CMAKE_CURRENT_SOURCE_DIR}/html/shell_custom.html" # (Optional) Custom HTML shell
#         # Pre-pack your assets directory into the WebAssembly virtual filesystem
#         "--preload-file" "${CMAKE_CURRENT_BINARY_DIR}/data.dat@/data.dat"
#     )
#
# endif()


if (BUILD_EXAMPLES)
    message(STATUS "Building Examples: Enabled")
    # add_subdirectory(examples/example1)
    # add_subdirectory(examples/example2)
    # add_subdirectory(examples/basic-example)
    # add_subdirectory(examples/simple-hierachy)
    # add_subdirectory(examples/global-transforms)
    # add_subdirectory(examples/animated-sprite)
    # add_subdirectory(examples/keyframe-animation)
    # add_subdirectory(examples/engine-variant)
    # add_subdirectory(examples/tweening)
    # add_subdirectory(examples/physics-server-test)
    # add_subdirectory(examples/event-bus)
    # add_subdirectory(examples/basic-ui)
    add_subdirectory(examples/opengl-render)

else()
    message(STATUS "Building Examples: Disabled")
endif()
# enable_testing()
# add_subdirectory(tests)
