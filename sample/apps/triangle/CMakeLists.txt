cmake_minimum_required(VERSION 3.21)

project(
    triangle_sample
    VERSION 0.1
    DESCRIPTION "triangle sample"
    LANGUAGES CXX
)

list(APPEND SRC_FILES
    main.cpp
    triangle_sample.cpp
    triangle_sample.h
)

add_executable(triangle_sample
    ${SRC_FILES}
)

target_include_directories(triangle_sample
    PUBLIC
    ${VKT_INCLUDE_DIRS}
    ${BASE_INCLUDE_DIRS}
)

target_link_libraries(triangle_sample
    PRIVATE
    glm
    vkt::vkt
    vkt_sample_base
    spdlog::spdlog_header_only
)

set(GLSLC_EXE $ENV{VULKAN_SDK}/bin/glslc)

if(CMAKE_GENERATOR STREQUAL "Ninja")
    set(SHADER_OUTPUT ${CMAKE_CURRENT_BINARY_DIR})
else()
    set(SHADER_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
endif()

if(EXISTS ${GLSLC_EXE})
    add_custom_command(TARGET triangle_sample POST_BUILD
        COMMAND ${GLSLC_EXE} ${CMAKE_CURRENT_SOURCE_DIR}/triangle.vert -o ${SHADER_OUTPUT}/triangle_vert.spv
    )
    add_custom_command(TARGET triangle_sample POST_BUILD
        COMMAND ${GLSLC_EXE} ${CMAKE_CURRENT_SOURCE_DIR}/triangle.frag -o ${SHADER_OUTPUT}/triangle_frag.spv
    )
else()
    # TODO: compile shader at not run time.
    file(COPY triangle_vert.spv DESTINATION ${SHADER_OUTPUT})
    file(COPY triangle_frag.spv DESTINATION ${SHADER_OUTPUT})

    # file(COPY triangle.vert DESTINATION ${SHADER_OUTPUT})
    # file(COPY triangle.frag DESTINATION ${SHADER_OUTPUT})
endif()

if(MSVC)
# TODO
elseif(APPLE)
    target_compile_options(triangle_sample
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