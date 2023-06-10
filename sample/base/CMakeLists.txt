cmake_minimum_required(VERSION 3.21)

project(
    vkt_sample_base
    VERSION 0.1
    DESCRIPTION "vkt sample base"
    LANGUAGES CXX
)

if(NOT ANDROID)
    find_package(SDL2 CONFIG REQUIRED)
endif()

list(APPEND SRC_FILES
    sample.cpp
    sample.h
    window.h
    file.cpp
    file.h
)

if(ANDROID)
    list(APPEND SRC_FILES
        window_android.cpp
    )
else()
    list(APPEND SRC_FILES
        window_sdl.cpp
    )
endif()

if(APPLE)
    list(APPEND SRC_FILES
        window_cocoa.mm
    )
elseif(MSVC)
    list(APPEND SRC_FILES
        window_win32.cpp
    )
endif()

add_library(vkt_sample_base STATIC ${SRC_FILES})
add_library(vkt::sample_base ALIAS vkt_sample_base)

if(NOT ANDROID)
    target_link_libraries(vkt_sample_base
        PUBLIC
        SDL2::SDL2 SDL2::SDL2main
    )
endif()

target_include_directories(vkt
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR} # for export
)