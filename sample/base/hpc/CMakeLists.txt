cmake_minimum_required(VERSION 3.21)

project(
    hpc
    VERSION 0.1
    DESCRIPTION "hardware performance counter"
    LANGUAGES CXX
)

find_package(hwcpipe CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

set(SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/sampler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/gpu.h

    ${CMAKE_CURRENT_SOURCE_DIR}/hpc.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/hpc.h
    ${CMAKE_CURRENT_SOURCE_DIR}/counter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/counter.h

    ${CMAKE_CURRENT_SOURCE_DIR}/mali/mali_sampler.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mali/mali_sampler.h
    ${CMAKE_CURRENT_SOURCE_DIR}/mali/mali_gpus.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mali/mali_gpus.h
    ${CMAKE_CURRENT_SOURCE_DIR}/mali/mali_gpu.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mali/mali_gpu.h
    ${CMAKE_CURRENT_SOURCE_DIR}/mali/mali_counter.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/mali/mali_counter.h
)

add_library(hpc STATIC ${SRC_FILES})
add_library(jipu::hpc ALIAS hpc)

target_include_directories(
    hpc PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/mali
)

target_link_libraries(hpc PRIVATE
    hwcpipe::device
    hwcpipe::hwcpipe
    hwcpipe::device_private
    spdlog::spdlog_header_only
)