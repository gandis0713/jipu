cmake_minimum_required(VERSION 3.21)

set(PRJ_NAME test_vkt)

find_package(Gtest CONFIG REQUIRED)

file(GLOB_RECURSE PRJ_SRCS LIST_DIRECTORIES false *.h *.cpp)

if(MSVC)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()

add_executable(${PRJ_NAME} ${PRJ_SRCS})

add_dependencies(${PRJ_NAME} vkt)
add_test(NAME ${PRJ_NAME} COMMAND ${PRJ_NAME})

target_link_libraries(${PRJ_NAME}
  PUBLIC
  Vulkan::Vulkan
  GTest::gmock GTest::gtest GTest::gmock_main GTest::gtest_main
  vkt
)

target_include_directories(${PRJ_NAME}
  PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../src/"
)

if(WIN32)
  get_property(TEST_TARGETS DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY TESTS)

  if(CMAKE_GENERATOR STREQUAL "Ninja")
    set(VKT_BUILD_DIR ${CMAKE_BINARY_DIR}/src)
  else()
    set(VKT_BUILD_DIR ${CMAKE_BINARY_DIR}/src/${CMAKE_BUILD_TYPE})
  endif()

  foreach(TEST_TARGET ${TEST_TARGETS})
    set_property(TEST "${TEST_TARGET}" PROPERTY ENVIRONMENT_MODIFICATION "PATH=path_list_append:${VKT_BUILD_DIR}")
  endforeach()
endif()