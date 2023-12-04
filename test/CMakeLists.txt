cmake_minimum_required(VERSION 3.21)

set(PRJ_NAME test_jipu)

find_package(Gtest CONFIG REQUIRED)
find_package(sdl2 CONFIG REQUIRED)

set(PRJ_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/copy_test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/copy_test.h
  ${CMAKE_CURRENT_SOURCE_DIR}/submit_test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/submit_test.h
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/base/test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/base/test.h
  ${CMAKE_CURRENT_SOURCE_DIR}/base/window_test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/base/window_test.h
)

if(APPLE)
  list(APPEND PRJ_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/base/window_test_cocoa.mm
  )
elseif(WIN32)
  list(APPEND PRJ_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/base/window_test_win32.cpp
  )
endif()

add_executable(${PRJ_NAME} ${PRJ_SRCS})
add_executable(jipu::test ALIAS test_jipu)
add_test(NAME ${PRJ_NAME} COMMAND ${PRJ_NAME})

target_include_directories(${PRJ_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${PRJ_NAME}
  PUBLIC
  jipu::jipu
  GTest::gmock
  GTest::gtest
  GTest::gmock_main
  GTest::gtest_main
  SDL2::SDL2
  SDL2::SDL2main
)

if(OFFSCREEN_TEST)
  target_compile_definitions(${PRJ_NAME}
    PRIVATE
    "OFFSCREEN_TEST"
  )
endif()