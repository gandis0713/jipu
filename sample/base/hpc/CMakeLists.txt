cmake_minimum_required(VERSION 3.21)

project(
    hpc
    VERSION 0.1
    DESCRIPTION "hardware performance counter"
    LANGUAGES CXX
)

find_package(hwcpipe CONFIG REQUIRED)

set(SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/sampler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sampler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gpu.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/gpu.h
    ${CMAKE_CURRENT_SOURCE_DIR}/hpc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hpc.h
    ${CMAKE_CURRENT_SOURCE_DIR}/handle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/handle.h
    ${CMAKE_CURRENT_SOURCE_DIR}/device.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/device.h
    ${CMAKE_CURRENT_SOURCE_DIR}/counter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/counter.h

    ${CMAKE_CURRENT_SOURCE_DIR}/mali/mali_sampler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mali/mali_sampler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/mali/mali_gpu.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mali/mali_gpu.h
    ${CMAKE_CURRENT_SOURCE_DIR}/mali/mali_counter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mali/mali_counter.h
)

add_library(hpc STATIC
    ${SRC_FILES}
)

message(STATUS "charlesaaaa")

target_link_libraries(hpc PRIVATE hwcpipe::device hwcpipe::hwcpipe hwcpipe::device_private)