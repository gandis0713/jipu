cmake_minimum_required(VERSION 3.21)

set(PRJ_NAME test_vkt)

find_package(Gtest CONFIG REQUIRED)

file(GLOB_RECURSE PRJ_SRCS LIST_DIRECTORIES false *.h *.cpp)

message(STATUS ${PRJ_SRCS})

if(MSVC)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()

add_executable(${PRJ_NAME} ${PRJ_SRCS})
add_dependencies(${PRJ_NAME} vkt)
add_test(NAME ${PRJ_NAME} COMMAND ${PRJ_NAME})

target_link_libraries(${PRJ_NAME}
  PUBLIC ${VKT_PROJECT_NAME}
  Vulkan::Vulkan
  GTest::gmock GTest::gtest GTest::gmock_main GTest::gtest_main
  vkt
)

target_include_directories(${PRJ_NAME}
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../src/"
)