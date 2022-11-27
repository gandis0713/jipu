cmake_minimum_required(VERSION 3.20)

# version
set(VKT_VERSION_MAJOR 0)
set(VKT_VERSION_MINOR 0)
set(VKT_VERSION_PATCH 1)

# initilize project.
set(VKT_PROJECT_NAME vkt)

# set options.
option(VKT_TEST "Test VKT" ON)

project(
    ${VKT_PROJECT_NAME}
    VERSION ${VKT_VERSION_MAJOR}.${VKT_VERSION_MINOR}.${VKT_VERSION_PATCH}
    DESCRIPTION "Vulkan Test"
    LANGUAGES CXX)

# set compile options.
set(CMAKE_CXX_STANDARD 17)

# set variables.
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(SAMPLE_DIR ${CMAKE_SOURCE_DIR}/samples)
set(TEST_DIR ${CMAKE_SOURCE_DIR}/test)

# set compile definitions.
add_compile_definitions(VKT_VERSION_MAJOR=${VKT_VERSION_MAJOR})
add_compile_definitions(VKT_VERSION_MINOR=${VKT_VERSION_MINOR})
add_compile_definitions(VKT_VERSION_PATCH=${VKT_VERSION_PATCH})

# find package
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
find_package(Vulkan REQUIRED)

# set build project list
add_subdirectory(${SRC_DIR})
add_subdirectory(${SAMPLE_DIR})

# add_subdirectory(${TEST_DIR})
