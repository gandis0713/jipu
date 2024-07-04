cmake_minimum_required(VERSION 3.21)

project(
    hpc-backend
    VERSION 0.0.1
    DESCRIPTION "HPC backend"
    LANGUAGES CXX
)

set(SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/instance.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/instance_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/instance_impl.h
    ${CMAKE_CURRENT_SOURCE_DIR}/source/handle.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/handle_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/handle_impl.h

    ${CMAKE_CURRENT_SOURCE_DIR}/source/device/adreno/adreno_instance_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/device/adreno/adreno_instance_impl.h

    ${CMAKE_CURRENT_SOURCE_DIR}/source/device/mali/mali_instance_impl.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/source/device/mali/mali_instance_impl.h

    ${CMAKE_CURRENT_SOURCE_DIR}/include/hpc/backend/handle.h
    ${CMAKE_CURRENT_SOURCE_DIR}/include/hpc/backend/instance.h
)

add_library(backend STATIC
    ${SRC_FILES}
)
add_library(hpc::backend ALIAS backend)

target_link_libraries(backend PRIVATE spdlog::spdlog_header_only)
target_include_directories(backend
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
    PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source>
)
