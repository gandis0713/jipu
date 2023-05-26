cmake_minimum_required(VERSION 3.12)

find_package(Vulkan REQUIRED)

file(GLOB_RECURSE SRC_FILES LIST_DIRECTORIES false ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

if(APPLE)
  file(GLOB_RECURSE MM_FILES LIST_DIRECTORIES false *.mm)
  list(APPEND SRC_FILES ${MM_FILES})

  file(GLOB_RECURSE REMOVE_FILES LIST_DIRECTORIES false *_windows.h *_windows.cpp)
  list(REMOVE_ITEM SRC_FILES ${REMOVE_FILES})
endif()

set(LIB_TYPE STATIC)

if(BUILD_SHARED_LIBS)
  set(LIB_TYPE SHARED)
endif()

add_library(vkt
  ${LIB_TYPE} # SHARED or STATIC
  ${SRC_FILES}
)
add_library(vkt::vkt ALIAS vkt)

# include directories
target_include_directories(vkt
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include # for export
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# link libraries
target_link_libraries(vkt
  PUBLIC
  Vulkan::Vulkan
  PRIVATE
  spdlog::spdlog_header_only
)

if(APPLE)
  target_link_libraries(vkt
    PRIVATE
    "-framework Cocoa"
  )
endif()

# compile options
if(MSVC)
# TODO
elseif(APPLE)
  target_compile_options(vkt
    PUBLIC
    -Wall
    -Werror
    -Wno-unused-parameter
    -Wno-unused-function
    -Wno-unused-but-set-variable
    -Wno-unused-variable

    -fvisibility=hidden
    -fvisibility-inlines-hidden

    PRIVATE
    -fno-rtti # -fno-exceptions
  )
endif()

# compile definitions
target_compile_definitions(vkt
  PRIVATE
  "VKT_IMPLEMENTATION"
)

if(BUILD_SHARED_LIBS)
  target_compile_definitions(vkt
    PRIVATE
    "VKT_SHARED_LIBRARY"
  )
endif()

# install
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION ${CMAKE_INSTALL_PREFIX})

install(TARGETS vkt
  INCLUDES DESTINATION ${CMAKE_INSTALL_PREFIX}/include
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
  LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
  ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)
