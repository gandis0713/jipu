cmake_minimum_required(VERSION 3.12)

# The information about project.
set(PRJ_NAME "triangle")
project(
  ${PRJ_NAME}
  VERSION 0.1
  DESCRIPTION "${PRJ_NAME} sample"
  LANGUAGES CXX
)

add_executable(${PRJ_NAME} main.cpp)
add_dependencies(${PRJ_NAME} vkt)

target_include_directories(
  ${PRJ_NAME}
  PUBLIC

  # ${Vulkan_INCLUDE_DIRS}
  ${VKT_INCLUDE_DIRS}
)

target_link_libraries(${PRJ_NAME}
  PRIVATE

  # ${Vulkan_LIBRARIES}
  Vulkan::Vulkan
  glfw
  # ${VKT_LIBRARIES}
  vkt
)

set(GLSLC_EXE $ENV{VULKAN_SDK}/bin/glslc)
if(CMAKE_GENERATOR STREQUAL "Ninja")
  set(SHADER_OUTPUT ${CMAKE_BINARY_DIR}/samples/basic/${PRJ_NAME})
else()
  set(SHADER_OUTPUT ${CMAKE_BINARY_DIR}/samples/basic/${PRJ_NAME}/${CMAKE_BUILD_TYPE})
endif()

add_custom_command(TARGET ${PRJ_NAME} POST_BUILD
  COMMAND ${GLSLC_EXE} ${CMAKE_CURRENT_SOURCE_DIR}/shader.vert -o ${SHADER_OUTPUT}/vert.spv
)
add_custom_command(TARGET ${PRJ_NAME} POST_BUILD
  COMMAND ${GLSLC_EXE} ${CMAKE_CURRENT_SOURCE_DIR}/shader.frag -o ${SHADER_OUTPUT}/frag.spv
)

# target_compile_options(${PRJ_NAME} PUBLIC -Wall -Werror -fno-rtti)