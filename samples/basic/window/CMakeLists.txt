cmake_minimum_required(VERSION 3.12)

# The information about project.
set(PROJECT_NAME "window")
project(
  ${PROJECT_NAME}
  VERSION 0.1
  DESCRIPTION "Vulkan Window Test"
  LANGUAGES CXX
)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME}
  PRIVATE

  # ${Vulkan_LIBRARIES}
  Vulkan::Vulkan
  glfw
  ${VKT_LIBRARIES}
)

target_include_directories(${PROJECT_NAME}
  PRIVATE

  # ${Vulkan_INCLUDE_DIRS}
  ${VKT_INCLUDE_DIRS}
)

# target_compile_options(triangle PUBLIC -Wall -Werror -fno-rtti)