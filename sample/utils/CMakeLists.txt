cmake_minimum_required(VERSION 3.21)

project(
    jipu_sample_utils
    VERSION 0.1
    DESCRIPTION "jipu sample utils"
    LANGUAGES CXX
)

find_package(spdlog CONFIG REQUIRED)

list(APPEND SRC_FILES
    GPUCounter.cpp
    GPUCounter.h
)

if(ANDROID)
    list(APPEND SRC_FILES
        AndroidGPUCounter.cpp
        AndroidGPUCounter.h
    )
endif()

add_library(jipu_sample_utils STATIC ${SRC_FILES})
add_library(jipu::sample_utils ALIAS jipu_sample_utils)

target_link_libraries(jipu_sample_utils
    PRIVATE
    spdlog::spdlog_header_only
)

target_include_directories(jipu
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)