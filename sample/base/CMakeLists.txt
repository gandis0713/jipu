cmake_minimum_required(VERSION 3.21)

project(
    vkt_sample_base
    VERSION 0.1
    DESCRIPTION "vkt sample base"
    LANGUAGES CXX
)

find_package(spdlog CONFIG REQUIRED)
find_package(Stb REQUIRED)
find_package(tinyobjloader CONFIG REQUIRED)
find_package(Ktx CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_path(TINYGLTF_INCLUDE
    NAMES "tiny_gltf.h"
    HINTS ${VCPKG_INSTALLED_DIR}
    REQUIRED
)

if(NOT ANDROID)
    find_package(SDL2 CONFIG REQUIRED)
endif()

list(APPEND SRC_FILES
    sample.cpp
    sample.h
    window.cpp
    window.h
    vertex.cpp
    vertex.h
    model.cpp
    model.h
    file.cpp
    file.h
    light.cpp
    light.h
    khronos_texture.cpp
    khronos_texture.h
    image.cpp
    image.h
)

if(ANDROID)
    list(APPEND SRC_FILES
        window_android.cpp
    )
else()
    list(APPEND SRC_FILES
        window_sdl.cpp
    )

    if(APPLE)
        list(APPEND SRC_FILES
            window_cocoa.mm
        )
    elseif(MSVC)
        list(APPEND SRC_FILES
            window_win32.cpp
        )
    endif()
endif()

add_library(vkt_sample_base STATIC ${SRC_FILES})
add_library(vkt::sample_base ALIAS vkt_sample_base)

target_include_directories(vkt_sample_base
    PRIVATE
    ${Stb_INCLUDE_DIR}
    ${TINYGLTF_INCLUDE}
)

target_link_libraries(vkt_sample_base
    PRIVATE
    tinyobjloader::tinyobjloader
    spdlog::spdlog_header_only
    glm::glm
    KTX::ktx
)

if(NOT ANDROID)
    target_link_libraries(vkt_sample_base
        PUBLIC
        SDL2::SDL2 SDL2::SDL2main
    )
else()
    find_package(game-activity REQUIRED CONFIG)
    find_library(log-lib log)
    target_link_libraries(vkt_sample_base
        PUBLIC
        game-activity::game-activity # game-activity::game-activity_static
        android
        ${log-lib}
    )
endif()

target_include_directories(vkt
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR} # for export
)