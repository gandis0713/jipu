cmake_minimum_required(VERSION 3.21)

project(
    triangle_sample
    VERSION 0.1
    DESCRIPTION "triangle sample"
    LANGUAGES CXX
)

find_package(glm CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

if(APPLE)
    # Scheme Environment should be set to Xcode project.
    set(CMAKE_XCODE_SCHEME_ENVIRONMENT
        VK_ICD_FILENAMES=$ENV{VULKAN_SDK}/share/vulkan/icd.d/MoltenVK_icd.json
        VK_LAYER_PATH=$ENV{VULKAN_SDK}/share/vulkan/explicit_layer.d
    )
endif()

if(WIN32)
    set(VKT_LIB_NAME "vkt.lib")
elseif(APPLE)
    set(VKT_LIB_NAME "libvkt.a")
endif()

set(VKT_LIBRARIES "${CMAKE_BINARY_DIR}/vkt/${CMAKE_BUILD_TYPE}/${VKT_LIB_NAME}")
set(VKT_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/vkt/inc/")
list(APPEND SRC_FILES
    main.cpp
    sample.cpp
    sample.h
    triangle_sample.cpp
    triangle_sample.h
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

add_executable(triangle_sample
    ${SRC_FILES}
)

target_include_directories(triangle_sample
    PUBLIC
    ${VKT_INCLUDE_DIRS}
)

target_link_libraries(triangle_sample
    PRIVATE
    glfw
    glm
    vkt::vkt
    spdlog::spdlog_header_only
)

set(GLSLC_EXE $ENV{VULKAN_SDK}/bin/glslc)

if(CMAKE_GENERATOR STREQUAL "Ninja")
    set(SHADER_OUTPUT ${CMAKE_CURRENT_BINARY_DIR})
else()
    set(SHADER_OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
endif()

message(STATUS "SHADER_OUTPUT : ${SHADER_OUTPUT}")

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