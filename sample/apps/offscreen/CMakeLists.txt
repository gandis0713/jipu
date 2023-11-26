cmake_minimum_required(VERSION 3.21)

project(
    offscreen
    VERSION 0.1
    DESCRIPTION "offscreen sample"
    LANGUAGES CXX
)

list(APPEND SRC_FILES
    offscreen_sample.h
    offscreen_sample.cpp
    main.cpp
)

if(ANDROID)
    add_library(offscreen SHARED
        ${SRC_FILES}
    )
else()
    add_executable(offscreen
        ${SRC_FILES}
    )
endif()

find_package(spdlog CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)

target_link_libraries(offscreen
    PRIVATE
    glm::glm
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
    offscreen.vert
    offscreen.frag
    onscreen.vert
    onscreen.frag
)

# convert shader to spirv
find_program(GLSLC_EXE NAMES glslc)

if(NOT ANDROID)
    # copy spirv to output
    set(SHADER_DIR ${CMAKE_CURRENT_SOURCE_DIR}/shaders)
    set(SHADER_DST ${BINARY_OUT})

    if(EXISTS ${GLSLC_EXE})
        function(run_glslc target shader src dst)
            add_custom_command(TARGET ${target} POST_BUILD
                COMMAND ${GLSLC_EXE} ${src}/${shader} -o ${dst}/${shader}.spv
            )
        endfunction()

        foreach(SHADER ${SHADERS})
            run_glslc(offscreen ${SHADER} ${SHADER_DIR} ${SHADER_DST})
            run_glslc(offscreen ${SHADER} ${SHADER_DIR} ${SHADER_DIR})
        endforeach(SHADER)
    else()
        function(copy_spv src dst)
            file(COPY $${src} DESTINATION ${dst})
        endfunction()

        foreach(SHADER ${SHADERS})
            copy_spv(${SHADER_DIR}/${SHADER}.spv ${SHADER_DST})
        endforeach(SHADER)
    endif()
endif()

if(MSVC)
# TODO
elseif(APPLE)
    target_compile_options(offscreen
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