cmake_minimum_required(VERSION 3.12)

find_package(GLFW3 REQUIRED)

set(BASIC_PHYSICAL_DEVICE ${CMAKE_CURRENT_SOURCE_DIR}/basic/physical_device)
set(BASIC_LOGICAL_DEVICE ${CMAKE_CURRENT_SOURCE_DIR}/basic/logical_device)
set(BASIC_WINDOW ${CMAKE_CURRENT_SOURCE_DIR}/basic/window)
set(BASIC_TRIANGLE ${CMAKE_CURRENT_SOURCE_DIR}/basic/triangle)

add_subdirectory(${BASIC_PHYSICAL_DEVICE})
add_subdirectory(${BASIC_LOGICAL_DEVICE})
add_subdirectory(${BASIC_WINDOW})
add_subdirectory(${BASIC_TRIANGLE})