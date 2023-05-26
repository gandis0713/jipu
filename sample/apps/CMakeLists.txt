cmake_minimum_required(VERSION 3.21)

find_package(glm CONFIG REQUIRED)

if(APPLE)
    # Scheme Environment should be set to Xcode project.
    set(CMAKE_XCODE_SCHEME_ENVIRONMENT
        VK_ICD_FILENAMES=$ENV{VULKAN_SDK}/share/vulkan/icd.d/MoltenVK_icd.json
        VK_LAYER_PATH=$ENV{VULKAN_SDK}/share/vulkan/explicit_layer.d
    )
endif()

add_subdirectory(triangle)