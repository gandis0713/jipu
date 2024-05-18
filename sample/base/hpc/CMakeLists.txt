cmake_minimum_required(VERSION 3.21)

project(
    hpc
    VERSION 0.1
    DESCRIPTION "hardware performance counter"
    LANGUAGES CXX
)

add_subdirectory(backend)
add_subdirectory(hpc)