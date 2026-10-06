cmake_minimum_required(VERSION 3.25)

cmake_path(GET CMAKE_CURRENT_SOURCE_DIR FILENAME EXAMPLE_NAME )

project(${EXAMPLE_NAME} LANGUAGES CXX)

file(GLOB_RECURSE SRCS "src/*.cpp")

MESSAGE(STATUS "Building Example: " ${PROJECT_NAME})

add_executable(${PROJECT_NAME} ${SRCS})

target_include_directories(${PROJECT_NAME} PRIVATE src)

target_link_libraries(${PROJECT_NAME} PRIVATE Varicle::Varicle)
