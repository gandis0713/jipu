cmake_minimum_required(VERSION 3.21)

set(PRJ_NAME test_vkt)

find_package(Gtest CONFIG REQUIRED)

file(GLOB_RECURSE PRJ_SRCS LIST_DIRECTORIES false ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(${PRJ_NAME} ${PRJ_SRCS})
add_dependencies(${PRJ_NAME} vkt)
add_test(NAME ${PRJ_NAME} COMMAND ${PRJ_NAME})

target_link_libraries(${PRJ_NAME}
  PUBLIC
  ${VKT_PROJECT_NAME}
  GTest::gmock
  GTest::gtest
  GTest::gmock_main
  GTest::gtest_main
)