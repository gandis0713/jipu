cmake_minimum_required(VERSION 3.12)

# initilize project.
set(PROJECT_NAME vks)

project(
  ${PROJECT_NAME}
  VERSION 0.1
  DESCRIPTION "Vulkan samples"
  LANGUAGES CXX)

# set compile options.
set(CMAKE_CXX_STANDARD 17)

# set variables.
set(SRC_DIR src)
set(SAMPLE_DIR samples)

# set compile definitions.
add_compile_definitions(IEL_NAME_SPACE=${PROJECT_NAME})

# find package
find_package(Vulkan REQUIRED)

# set build project list
add_subdirectory(${SRC_DIR})
add_subdirectory(${SAMPLE_DIR})