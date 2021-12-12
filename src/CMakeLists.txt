cmake_minimum_required(VERSION 3.12)

file(GLOB_RECURSE SRC_FILES LIST_DIRECTORIES false *.h *.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

add_library(${VKS_PROJECT_NAME} STATIC ${SRC_FILES})

target_link_libraries(${VKS_PROJECT_NAME})