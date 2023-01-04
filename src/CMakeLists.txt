cmake_minimum_required(VERSION 3.12)

file(GLOB_RECURSE SRC_FILES LIST_DIRECTORIES false *.h *.cpp *.mm)

# include_directories(
# ${CMAKE_CURRENT_SOURCE_DIR}/inc
# ${Vulkan_INCLUDE_DIR}
# )
add_library(${VKT_PROJECT_NAME} STATIC ${SRC_FILES})

target_include_directories(${VKT_PROJECT_NAME}
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${VKT_PROJECT_NAME}
  PUBLIC
  "-framework Metal"
  Vulkan::Vulkan
)