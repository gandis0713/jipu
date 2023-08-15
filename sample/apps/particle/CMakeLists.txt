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

# define shader input
set(SHADERS_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/shaders)

# convert shader to spirv
find_program(GLSLC_EXE NAMES glslc)

if(EXISTS ${GLSLC_EXE})
    add_custom_command(TARGET particle PRE_BUILD
        COMMAND ${GLSLC_EXE} ${SHADERS_INPUT}/particle.vert -o ${SHADERS_INPUT}/particle.vert.spv
    )
    add_custom_command(TARGET particle PRE_BUILD
        COMMAND ${GLSLC_EXE} ${SHADERS_INPUT}/particle.frag -o ${SHADERS_INPUT}/particle.frag.spv
    )
endif()

# copy spirv to output
if(CMAKE_GENERATOR STREQUAL "Ninja")
    set(BINARY_OUT ${CMAKE_CURRENT_BINARY_DIR})
else()
    set(BINARY_OUT ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
endif()

if(NOT ANDROID)
    set(SHADER_OUTPUT ${BINARY_OUT})

    # TODO: copy after creating *spv.
    file(COPY ${SHADERS_INPUT}/particle.vert.spv DESTINATION ${SHADER_OUTPUT})
    file(COPY ${SHADERS_INPUT}/particle.frag.spv DESTINATION ${SHADER_OUTPUT})
endif()

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