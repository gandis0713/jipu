cmake_minimum_required(VERSION 3.21)

# set options.
option(VKT_TEST "VKT Test" ON)

project(
  vkt
  VERSION 0.0.1
  DESCRIPTION "Vulkan Test"
  LANGUAGES CXX
)

# set compile options.
set(CMAKE_CXX_STANDARD 20)

if(MSVC)
  add_compile_options(
    /MP
  )
elseif(APPLE)
  enable_language(OBJCXX)
endif()

# find package
find_package(spdlog CONFIG REQUIRED)

add_subdirectory(src)
add_subdirectory(app)
add_dependencies(app vkt::vkt)

if(VKT_TEST)
  enable_testing()
  add_subdirectory(test)
  add_dependencies(test_vkt vkt::vkt)
endif()
