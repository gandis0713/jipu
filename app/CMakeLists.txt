cmake_minimum_required(VERSION 3.21)

# The information about project.
set(PRJ_NAME "app")
project(
    ${PRJ_NAME}
    VERSION 0.1
    DESCRIPTION "${PRJ_NAME} sample"
    LANGUAGES CXX
)

find_package(glfw3 CONFIG REQUIRED)

if(APPLE)
    # Scheme Environment should be set to Xcode project.
    set(CMAKE_XCODE_SCHEME_ENVIRONMENT
        VK_ICD_FILENAMES=$ENV{VULKAN_SDK}/share/vulkan/icd.d/MoltenVK_icd.json
        VK_LAYER_PATH=$ENV{VULKAN_SDK}/share/vulkan/explicit_layer.d
    )
endif()

if(WIN32)
    set(VKT_LIBRARIES "${CMAKE_BINARY_DIR}/src/${CMAKE_BUILD_TYPE}/vkt.lib")
    set(VKT_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/src/inc/")
elseif(APPLE)
    set(VKT_LIBRARIES "${CMAKE_BINARY_DIR}/src/${CMAKE_BUILD_TYPE}/libvkt.a")
    set(VKT_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/src/inc/")
endif()

list(APPEND SRC_FILES
    main.cpp
    application.cpp
    application.h
    window.h
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

add_executable(${PRJ_NAME}
    ${SRC_FILES}
)

add_dependencies(${PRJ_NAME} ${VKT_PROJECT_NAME})

target_include_directories(${PRJ_NAME}
    PUBLIC
    ${VKT_INCLUDE_DIRS}
)

target_link_libraries(${PRJ_NAME}
    PRIVATE
    glfw
    ${VKT_PROJECT_NAME}
    spdlog::spdlog_header_only
)

set(GLSLC_EXE $ENV{VULKAN_SDK}/bin/glslc)

if(CMAKE_GENERATOR STREQUAL "Ninja")
    set(SHADER_OUTPUT ${CMAKE_BINARY_DIR}/${PRJ_NAME})
else()
    set(SHADER_OUTPUT ${CMAKE_BINARY_DIR}/${PRJ_NAME}/${CMAKE_BUILD_TYPE})
endif()

if(EXISTS ${GLSLC_EXE})
    add_custom_command(TARGET ${PRJ_NAME} POST_BUILD
        COMMAND ${GLSLC_EXE} ${CMAKE_CURRENT_SOURCE_DIR}/triangle.vert -o ${SHADER_OUTPUT}/triangle_vert.spv
    )
    add_custom_command(TARGET ${PRJ_NAME} POST_BUILD
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
    target_compile_options(${PRJ_NAME}
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