cmake_minimum_required(VERSION 3.21)

project(
    vkt_sample_base
    VERSION 0.1
    DESCRIPTION "vkt sample base"
    LANGUAGES CXX
)

find_package(glfw3 CONFIG REQUIRED)

list(APPEND SRC_FILES
    sample.cpp
    sample.h
    window.h
    file.cpp
    file.h
)

if(WIN32)
    list(APPEND SRC_FILES
        window_win32.cpp
    )
elseif(APPLE)
    list(APPEND SRC_FILES
        window_cocoa.mm
    )
endif()

add_library(vkt_sample_base STATIC ${SRC_FILES})

target_link_libraries(vkt_sample_base
    PUBLIC
    glfw
)

target_include_directories(vkt
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR} # for export
)