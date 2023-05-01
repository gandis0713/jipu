cmake_minimum_required(VERSION 3.12)

find_package(Vulkan REQUIRED)

file(GLOB_RECURSE SRC_FILES LIST_DIRECTORIES false ENDF6_SRC ${CMAKE_SOURCE_DIR} *.h *.cpp)

if(APPLE)
  file(GLOB_RECURSE MM_FILES LIST_DIRECTORIES false *.mm)
  list(APPEND SRC_FILES ${MM_FILES})

  file(GLOB_RECURSE REMOVE_FILES LIST_DIRECTORIES false *_windows.h *_windows.cpp)
  list(REMOVE_ITEM SRC_FILES ${REMOVE_FILES})
endif()

add_library(${VKT_PROJECT_NAME} 
  STATIC
  ${SRC_FILES}
)

target_include_directories(${VKT_PROJECT_NAME}
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${VKT_PROJECT_NAME}
  PUBLIC
  Vulkan::Vulkan
  PRIVATE
  spdlog::spdlog_header_only
)

if(APPLE)
  target_link_libraries(${VKT_PROJECT_NAME}
    PUBLIC
    "-framework Cocoa"
  )
endif()

if(MSVC)
  # TODO
elseif(APPLE)
target_compile_options(${VKT_PROJECT_NAME}
  PUBLIC
  -Wall
  -Werror
  -Wno-unused-parameter
  -Wno-unused-function
  -Wno-unused-but-set-variable
  -Wno-unused-variable

  PRIVATE
  -fno-rtti # -fno-exceptions
)
endif()
