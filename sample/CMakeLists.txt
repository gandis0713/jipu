cmake_minimum_required(VERSION 3.21)

project(
    vkt_sample
    VERSION 0.1
    DESCRIPTION "vkt sample"
    LANGUAGES CXX
)

find_package(spdlog CONFIG REQUIRED)

add_subdirectory(base)
add_subdirectory(apps)