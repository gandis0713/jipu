# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.21.0)

# Declares and names the project.
project("vulkan_test")

set(CMAKE_CXX_STANDARD 20)

set(ROOT_DIR "${CMAKE_SOURCE_DIR}/../../../../../..")
set(TRIANGLE_DIR "${ROOT_DIR}/sample/apps/triangle")
find_package(game-activity REQUIRED CONFIG)
find_library(log-lib log)

add_library(vulkan_test
    SHARED
        ${TRIANGLE_DIR}/main.cpp
)


#add_library(vkt::sample_base IMPORTED)
#set_target_properties(vkt::sample_base PROPERTIES
#        INTERFACE_INCLUDE_DIRECTORIES "${ROOT_DIR}/install/${ANDROID_ABI}/debug/include")

add_library(vkt::vkt SHARED IMPORTED)
set_target_properties(vkt::vkt PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${ROOT_DIR}/install/${ANDROID_ABI}/debug/include")

set_target_properties(vkt::vkt PROPERTIES
        IMPORTED_LOCATION_DEBUG "${ROOT_DIR}/install/${ANDROID_ABI}/debug/lib/libvkt.so")
set_property(TARGET vkt::vkt APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} \
                     -DVK_USE_PLATFORM_ANDROID_KHR")

target_include_directories(vulkan_test
        PUBLIC
        ${VTK_DIR}/include # for export
        PRIVATE
        ${VTK_DIR}
)

target_link_libraries(vulkan_test
        PUBLIC
        game-activity::game-activity # game-activity::game-activity_static
        android
        vkt::vkt
        ${log-lib}
)