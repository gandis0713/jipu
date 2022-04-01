cmake_minimum_required(VERSION 3.12)

# The information about project.
project(
triangle
VERSION 0.1
DESCRIPTION "Triangle sample"
LANGUAGES CXX
)

add_executable(triangle main.cpp)

target_include_directories(
  triangle
  PUBLIC
	${Vulkan_INCLUDE_DIRS}
  ${GLFW3_INCLUDE_DIR}
)

target_link_libraries(triangle
	${Vulkan_LIBRARIES}
  ${GLFW3_LIBRARY}
)

# target_compile_options(triangle PUBLIC -Wall -Werror -fno-rtti)