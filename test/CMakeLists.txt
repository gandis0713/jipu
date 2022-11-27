cmake_minimum_required(VERSION 3.21)

SET(VKT_TEST_BINARY test_vkt)

file(GLOB_RECURSE VKT_TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)

if(MSVC)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
endif()

add_executable(${VKT_TEST_BINARY} ${VKT_TEST_SOURCES})
add_test(NAME ${VKT_TEST_BINARY} COMMAND ${VKT_TEST_BINARY})
find_package(Gtest CONFIG REQUIRED)
target_link_libraries(${VKT_TEST_BINARY} PUBLIC ${VKT_PROJECT_NAME} Vulkan::Vulkan GTest::gmock GTest::gtest GTest::gmock_main GTest::gtest_main)
target_include_directories(${VKT_TEST_BINARY} PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../src/inc/")