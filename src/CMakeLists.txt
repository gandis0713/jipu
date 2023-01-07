cmake_minimum_required(VERSION 3.12)

file(GLOB_RECURSE SRC_FILES LIST_DIRECTORIES false *.h *.cpp)

if(APPLE)
  file(GLOB_RECURSE MM_FILES LIST_DIRECTORIES false *.mm)
  list(APPEND SRC_FILES ${MM_FILES})
endif()

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
  Vulkan::Vulkan
)

if(APPLE)
  target_link_libraries(${VKT_PROJECT_NAME}
    PUBLIC
    "-framework Metal"
  )
endif()
