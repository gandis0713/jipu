cmake_minimum_required(VERSION 3.12)

find_package(glfw3 CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

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

set(BASIC_PHYSICAL_DEVICE ${CMAKE_CURRENT_SOURCE_DIR}/basic/physical_device)
set(BASIC_LOGICAL_DEVICE ${CMAKE_CURRENT_SOURCE_DIR}/basic/logical_device)
set(BASIC_WINDOW ${CMAKE_CURRENT_SOURCE_DIR}/basic/window)
set(BASIC_SWAPCHAIN ${CMAKE_CURRENT_SOURCE_DIR}/basic/swapchain)
set(BASIC_COMMANDS ${CMAKE_CURRENT_SOURCE_DIR}/basic/commands)
set(BASIC_SYNCHRONIZE ${CMAKE_CURRENT_SOURCE_DIR}/basic/synchronize)
set(BASIC_IMAGE_VIEW ${CMAKE_CURRENT_SOURCE_DIR}/basic/image_view)
set(BASIC_FRAMEBUFFER ${CMAKE_CURRENT_SOURCE_DIR}/basic/framebuffer)
set(BASIC_RENDER_PASS ${CMAKE_CURRENT_SOURCE_DIR}/basic/render_pass)
set(BASIC_TRIANGLE ${CMAKE_CURRENT_SOURCE_DIR}/basic/triangle)

add_subdirectory(${BASIC_PHYSICAL_DEVICE})
add_subdirectory(${BASIC_LOGICAL_DEVICE})
add_subdirectory(${BASIC_WINDOW})
add_subdirectory(${BASIC_SWAPCHAIN})
add_subdirectory(${BASIC_COMMANDS})
add_subdirectory(${BASIC_SYNCHRONIZE})
add_subdirectory(${BASIC_IMAGE_VIEW})
add_subdirectory(${BASIC_FRAMEBUFFER})
add_subdirectory(${BASIC_RENDER_PASS})
add_subdirectory(${BASIC_TRIANGLE})