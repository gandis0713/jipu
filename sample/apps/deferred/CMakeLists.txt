cmake_minimum_required(VERSION 3.21)

project(
    deferred
    VERSION 0.1
    DESCRIPTION "deferred sample"
    LANGUAGES CXX
)

list(APPEND SRC_FILES
    main.cpp
)

if(ANDROID)
    add_library(deferred SHARED
        ${SRC_FILES}
    )
else()
    add_executable(deferred
        ${SRC_FILES}
    )
endif()

find_package(spdlog CONFIG REQUIRED)

target_link_libraries(deferred
    PRIVATE
    jipu::jipu
    jipu::sample_base
    spdlog::spdlog_header_only
)

if(CMAKE_GENERATOR STREQUAL "Ninja")
    set(BINARY_OUT ${CMAKE_CURRENT_BINARY_DIR})
else()
    set(BINARY_OUT ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
endif()

set(SHADERS
    composition.vert
    composition.frag
    offscreen.vert
    offscreen.frag
)

# convert shader to spirv
find_program(GLSLC_EXE NAMES glslc)

if(EXISTS ${GLSLC_EXE})
    if(NOT ANDROID)
        # define shader input
        set(SHADERS_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/shaders)

        # copy spirv to output
        set(SHADER_SRC ${CMAKE_CURRENT_SOURCE_DIR}/shaders/)
        set(SHADER_DST ${BINARY_OUT})

        function(run_glslc target shader src dst)
            add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${GLSLC_EXE} ${src}/${shader} -o ${dst}/${shader}.spv
            )
        endfunction()

        foreach(SHADER ${SHADERS})
            run_glslc(deferred ${SHADER} ${SHADER_SRC} ${SHADER_DST})
        endforeach(SHADER)
    endif()
endif()

set(RESOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/res)
set(RESOURCE_OUT ${BINARY_OUT})
file(COPY ${RESOURCE_DIR}/armor.gltf DESTINATION ${RESOURCE_OUT})
file(COPY ${RESOURCE_DIR}/colormap_rgba.ktx DESTINATION ${RESOURCE_OUT})
file(COPY ${RESOURCE_DIR}/normalmap_rgba.ktx DESTINATION ${RESOURCE_OUT})

if(MSVC)
# TODO
elseif(APPLE)
    target_compile_options(deferred
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