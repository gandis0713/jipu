cmake_minimum_required(VERSION 3.21)

project(
    vkt_sample
    VERSION 0.1
    DESCRIPTION "vkt sample"
    LANGUAGES CXX
)

# find_package(glm CONFIG REQUIRED)

# if(APPLE)
# # Scheme Environment should be set to Xcode project.
# set(CMAKE_XCODE_SCHEME_ENVIRONMENT
# VK_ICD_FILENAMES=$ENV{VULKAN_SDK}/share/vulkan/icd.d/MoltenVK_icd.json
# VK_LAYER_PATH=$ENV{VULKAN_SDK}/share/vulkan/explicit_layer.d
# )
# endif()

# if(WIN32)
# set(VKT_LIB_NAME "vkt.lib")
# elseif(APPLE)
# set(VKT_LIB_NAME "libvkt.a")
# endif()

# set(VKT_LIBRARIES "${CMAKE_BINARY_DIR}/vkt/${CMAKE_BUILD_TYPE}/${VKT_LIB_NAME}")
# set(VKT_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/vkt/inc/")
add_subdirectory(apps)