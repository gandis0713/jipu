cmake_minimum_required(VERSION 3.12)

# The information about project.
set(PRJ_NAME "window")
project(
  ${PRJ_NAME}
  VERSION 0.1
  DESCRIPTION "Vulkan Window Test"
  LANGUAGES CXX
)

add_executable(${PRJ_NAME} main.cpp)
add_dependencies(${PRJ_NAME} vkt)

target_link_libraries(${PRJ_NAME}
  PRIVATE

  # ${Vulkan_LIBRARIES}
  Vulkan::Vulkan
  glfw

  # ${VKT_LIBRARIES}
  vkt
  spdlog::spdlog_header_only
)

target_include_directories(${PRJ_NAME}
  PRIVATE

  # ${Vulkan_INCLUDE_DIRS}
  ${VKT_INCLUDE_DIRS}
)

# target_compile_options(triangle PUBLIC -Wall -Werror -fno-rtti)