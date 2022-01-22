cmake_minimum_required(VERSION 3.12)


# version
set(VKS_VERSION_MAJOR 0)
set(VKS_VERSION_MINOR 0)
set(VKS_VERSION_PATCH 1)

# initilize project.
set(VKS_PROJECT_NAME vks)

# set options.
option(VKS_TEST "Test VKS" ON)

project(
  ${VKS_PROJECT_NAME}
  VERSION ${VKS_VERSION_MAJOR}.${VKS_VERSION_MINOR}.${VKS_VERSION_PATCH}
  DESCRIPTION "Vulkan samples"
  LANGUAGES CXX)

# set compile options.
set(CMAKE_CXX_STANDARD 17)

# set variables.
set(SRC_DIR src)
set(SAMPLE_DIR samples)
set(TEST_DIR test)
set(THIRD_PARTY_DIR third_party)
set(IEL_DIR "${THIRD_PARTY_DIR}/iel")
set(GTEST_DIR "${THIRD_PARTY_DIR}/googletest")

# set compile definitions.
add_compile_definitions(VKS_VERSION_MAJOR=${VKS_VERSION_MAJOR})
add_compile_definitions(VKS_VERSION_MINOR=${VKS_VERSION_MINOR})
add_compile_definitions(VKS_VERSION_PATCH=${VKS_VERSION_PATCH})

# find package
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
find_package(Vulkan REQUIRED)

# set build project list
add_subdirectory(${SRC_DIR})
add_subdirectory(${SAMPLE_DIR})
add_subdirectory(${TEST_DIR})
add_subdirectory(${IEL_DIR})
add_subdirectory(${GTEST_DIR})