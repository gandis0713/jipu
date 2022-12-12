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
  ${PROJECT_NAME}
  PUBLIC

  # ${Vulkan_INCLUDE_DIRS}
  ${VKT_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE

  # ${Vulkan_LIBRARIES}
  Vulkan::Vulkan
  glfw
  ${VKT_LIBRARIES}
)

set(GLSLC_EXE $ENV{VULKAN_SDK}/bin/glslc)
add_custom_command(TARGET triangle POST_BUILD
  COMMAND ${GLSLC_EXE} ${CMAKE_CURRENT_SOURCE_DIR}/shader.vert -o ${CMAKE_BINARY_DIR}/samples/basic/triangle/${CMAKE_BUILD_TYPE}/vert.spv
)
add_custom_command(TARGET triangle POST_BUILD
  COMMAND ${GLSLC_EXE} ${CMAKE_CURRENT_SOURCE_DIR}/shader.frag -o ${CMAKE_BINARY_DIR}/samples/basic/triangle/${CMAKE_BUILD_TYPE}/frag.spv
)

# target_compile_options(triangle PUBLIC -Wall -Werror -fno-rtti)