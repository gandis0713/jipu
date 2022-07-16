cmake_minimum_required(VERSION 3.12)

# The information about project.

set(PROJECT_NAME "image_view")
project(
${PROJECT_NAME}
VERSION 0.1
DESCRIPTION "Vulkan Image View Test"
LANGUAGES CXX
)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
	${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
	${Vulkan_LIBRARIES}
  glfw
)

# target_compile_options(triangle PUBLIC -Wall -Werror -fno-rtti)