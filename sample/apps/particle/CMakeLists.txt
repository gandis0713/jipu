cmake_minimum_required(VERSION 3.21)

project(
    particle
    VERSION 0.1
    DESCRIPTION "particle sample"
    LANGUAGES CXX
)

list(APPEND SRC_FILES
    main.cpp
)

if(ANDROID)
    add_library(particle SHARED
        ${SRC_FILES}
    )
else()
    add_executable(particle
        ${SRC_FILES}
    )
endif()

find_package(spdlog CONFIG REQUIRED)

target_link_libraries(particle
    PRIVATE
    vkt::vkt
    vkt::sample_base
    spdlog::spdlog_header_only
)

if(MSVC)
# TODO
elseif(APPLE)
    target_compile_options(particle
        PUBLIC
        -Wall
        -Werror
        -Wno-unused-parameter
        -Wno-unused-function
        -Wno-unused-but-set-variable
        -Wno-unused-variable

        PRIVATE
        -fno-rtti # -fno-exceptions
    )
endif()