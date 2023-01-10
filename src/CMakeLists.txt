cmake_minimum_required(VERSION 3.12)

file(GLOB_RECURSE SRC_FILES LIST_DIRECTORIES false *.h *.cpp)

if(APPLE)
  file(GLOB_RECURSE MM_FILES LIST_DIRECTORIES false *.mm)
  list(APPEND SRC_FILES ${MM_FILES})
endif()

# set_target_properties(${VKT_PROJECT_NAME} PROPERTIES CXX_VISIBILITY_PRESET hidden)
add_library(${VKT_PROJECT_NAME} SHARED ${SRC_FILES})

if(VKT_SHARED_LIB)
  add_compile_definitions(VKT_SHARED_LIBRARY) # add_definitions(-DVKT_SHARED_LIBRARY="1")
endif()

target_include_directories(${VKT_PROJECT_NAME}
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(${VKT_PROJECT_NAME}
  PUBLIC
  Vulkan::Vulkan
)

if(APPLE)
  target_link_libraries(${VKT_PROJECT_NAME}
    PUBLIC
    "-framework Metal"
  )
endif()

target_compile_options(${VKT_PROJECT_NAME}
  PUBLIC
  "-fvisibility=hidden"
  "-fvisibility-inlines-hidden"
)