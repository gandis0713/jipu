cmake_minimum_required(VERSION 3.21)

# version
set(VKT_VERSION_MAJOR 0)
set(VKT_VERSION_MINOR 0)
set(VKT_VERSION_PATCH 1)

# initilize project.
set(VKT_PROJECT_NAME vkt)

# set options.
option(VKT_TEST "VKT Test" ON)
option(VKT_SAMPLES "VKT Samples" ON)

project(
  ${VKT_PROJECT_NAME}
  VERSION ${VKT_VERSION_MAJOR}.${VKT_VERSION_MINOR}.${VKT_VERSION_PATCH}
  DESCRIPTION "Vulkan Test"
  LANGUAGES CXX
)

# set compile options.
set(CMAKE_CXX_STANDARD 17)

# set compile definitions.
add_compile_definitions(VKT_VERSION_MAJOR=${VKT_VERSION_MAJOR})
add_compile_definitions(VKT_VERSION_MINOR=${VKT_VERSION_MINOR})
add_compile_definitions(VKT_VERSION_PATCH=${VKT_VERSION_PATCH})

# find package
find_package(Vulkan REQUIRED)

set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
add_subdirectory(${SRC_DIR})

if(VKT_TEST)
  set(TEST_DIR ${CMAKE_SOURCE_DIR}/test)

  enable_testing()
  add_subdirectory(${TEST_DIR})
endif()

if(VKT_SAMPLES)
  set(SAMPLE_DIR ${CMAKE_SOURCE_DIR}/samples)
  add_subdirectory(${SAMPLE_DIR})
endif()