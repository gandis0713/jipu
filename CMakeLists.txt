cmake_minimum_required(VERSION 3.21)

# set options.
option(VKT_TEST "VKT Test" OFF)

# set VCPKG_TARGET_TRIPLET
if(CMAKE_TOOLCHAIN_FILE MATCHES "vcpkg.cmake")
  if(ANDROID_ABI MATCHES "arm64-v8a")
    set(VCPKG_TARGET_TRIPLET "arm64-android")
  elseif(ANDROID_ABI MATCHES "armeabi-v7a")
    set(VCPKG_TARGET_TRIPLET "arm-neon-android")
  elseif(ANDROID_ABI MATCHES "x86_64")
    set(VCPKG_TARGET_TRIPLET "x64-android")
  elseif(ANDROID_ABI MATCHES "x86")
    set(VCPKG_TARGET_TRIPLET "x86-android")
  endif()
endif()

project(
  vkt
  VERSION 0.0.1
  DESCRIPTION "Vulkan Test"
  LANGUAGES CXX
)

# set compile options.
set(CMAKE_CXX_STANDARD 20)

if(MSVC)
  add_compile_options(
    /MP
  )
elseif(APPLE)
  enable_language(OBJCXX)
endif()

add_subdirectory(vkt)
add_subdirectory(sample)

if(VKT_TEST)
  enable_testing()
  add_subdirectory(test)
  add_dependencies(test_vkt vkt::vkt)
endif()

# runtime output
if(NOT ANDROID)
  set_target_properties(vkt PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/app
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/app
  )
endif()
