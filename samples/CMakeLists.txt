cmake_minimum_required(VERSION 3.12)

find_package(GLFW3 REQUIRED)

set(BASIC_TRIANGLE ${CMAKE_CURRENT_SOURCE_DIR}/basic/triangle)
add_subdirectory(${BASIC_TRIANGLE})