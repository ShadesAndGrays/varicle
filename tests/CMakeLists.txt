cmake_minimum_required(VERSION 3.20)

add_executable(math_test math_test.cpp)

target_include_directories(math_test
    PRIVATE
    ${CMAKE_SOURCE_DIR}
)

add_test(NAME math_test COMMAND math_test)
