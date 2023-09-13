cmake_minimum_required(VERSION 3.21)

set(PRJ_NAME test_vkt)

find_package(Gtest CONFIG REQUIRED)
find_package(sdl2 CONFIG REQUIRED)

set(PRJ_SRCS
  ${CMAKE_CURRENT_SOURCE_DIR}/particle_test.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

if(APPLE)
  list(APPEND PRJ_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/window/window_test_cocoa.mm
    ${CMAKE_CURRENT_SOURCE_DIR}/window/window_test.h
  )
elseif(WIN32)
  list(APPEND PRJ_SRCS
    ${CMAKE_CURRENT_SOURCE_DIR}/window/window_test_win32.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/window/window_test.h
  )
endif()

add_executable(${PRJ_NAME} ${PRJ_SRCS})
add_executable(vkt::test ALIAS test_vkt)
add_test(NAME ${PRJ_NAME} COMMAND ${PRJ_NAME})

target_include_directories(${PRJ_NAME} PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${PRJ_NAME}
  PUBLIC
  vkt::vkt
  GTest::gmock
  GTest::gtest
  GTest::gmock_main
  GTest::gtest_main
  SDL2::SDL2
  SDL2::SDL2main
)