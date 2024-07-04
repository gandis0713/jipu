cmake_minimum_required(VERSION 3.21)

set(SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/instance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/instance_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/instance_impl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/source/handle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/handle_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/handle_impl.h

    ${CMAKE_CURRENT_SOURCE_DIR}/include/device/handle.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/device/instance.h
)

add_library(device STATIC
    ${SRC_FILES}
)
add_library(hpc::device ALIAS device)

target_link_libraries(device PRIVATE spdlog::spdlog_header_only)
target_include_directories(device
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
