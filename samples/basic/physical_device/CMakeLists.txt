cmake_minimum_required(VERSION 3.12)

# The information about project.

set(PROJECT_NAME "physical_device")
project(
${PROJECT_NAME}
VERSION 0.1
DESCRIPTION "Physical Device test"
LANGUAGES CXX
)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
	${Vulkan_INCLUDE_DIRS}
  ${GLFW3_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME}
	${Vulkan_LIBRARIES}
  ${GLFW3_LIBRARY}
)

# target_compile_options(triangle PUBLIC -Wall -Werror -fno-rtti)