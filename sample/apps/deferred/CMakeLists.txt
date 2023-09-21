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
    vkt::vkt
    vkt::sample_base
    spdlog::spdlog_header_only
)

# define shader input
set(SHADERS_INPUT ${CMAKE_CURRENT_SOURCE_DIR}/shaders)

# convert shader to spirv
find_program(GLSLC_EXE NAMES glslc)

if(EXISTS ${GLSLC_EXE})
    file(GLOB_RECURSE SHADERS $${SHADERS_INPUT} "*.vert" "*.frag")

    function(run_glslc target shader_name)
        add_custom_command(TARGET ${target} PRE_BUILD
            COMMAND ${GLSLC_EXE} ${SHADERS_INPUT}/${shader_name}.vert -o ${SHADERS_INPUT}/${shader_name}.vert.spv
        )
    endfunction()

    foreach(SHADER ${SHADERS})
        run_glslc(deferred ${SHADER})
    endforeach(SHADER)
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
    file(GLOB_RECURSE SPVS $${SHADERS_INPUT} "*.spv")

    foreach(SPV ${SPVS})
        file(COPY ${SPV} DESTINATION ${SHADER_OUTPUT})
    endforeach()
endif()

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