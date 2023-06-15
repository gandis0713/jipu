cmake_minimum_required(VERSION 3.12)

# find package
find_package(Vulkan REQUIRED)
find_package(spdlog CONFIG REQUIRED)

set(SRC_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/pipeline_layout.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/command_encoder.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/texture.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/device.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/command_buffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/pipeline.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/buffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/swapchain.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/driver.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_surface.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_render_pass.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_synchronization.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_api.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_driver.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_pipeline.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_command_encoder.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_pipeline_layout.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_framebuffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_shader_module.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_memory.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_swapchain.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_memory_allocator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_texture.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_queue.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_physical_device.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_texture_view.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_command_buffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_buffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_device.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/queue.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/texture_view.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/physical_device.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/surface.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/shader_module.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/utils/dylib.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_command_buffer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_shader_module.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_surface.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_api.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_device.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_synchronization.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_pipeline.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_texture_view.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_driver.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_framebuffer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_physical_device.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_swapchain.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_buffer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_pipeline_layout.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_memory.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_command_encoder.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_texture.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_queue.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_memory_allocator.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_render_pass.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/export.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/vkt/gpu/device.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/vkt/gpu/shader_module.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/vkt/gpu/surface.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/vkt/gpu/texture_view.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/vkt/gpu/swapchain.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/vkt/gpu/driver.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/vkt/gpu/command_buffer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/vkt/gpu/buffer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/vkt/gpu/pipeline.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/vkt/gpu/texture.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/vkt/gpu/command_encoder.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/vkt/gpu/physical_device.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/vkt/gpu/queue.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/vkt/gpu/pipeline_layout.h
  ${CMAKE_CURRENT_SOURCE_DIR}/utils/fmt.h
  ${CMAKE_CURRENT_SOURCE_DIR}/utils/dylib.h
  ${CMAKE_CURRENT_SOURCE_DIR}/utils/log.h
  ${CMAKE_CURRENT_SOURCE_DIR}/utils/cast.h
  ${CMAKE_CURRENT_SOURCE_DIR}/utils/assert.h
  ${CMAKE_CURRENT_SOURCE_DIR}/utils/hash.h
)

if(APPLE)
  list(APPEND SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_surface_macos.mm
  )
elseif(MSVC)
  list(APPEND SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_surface_window.cpp
  )
elseif(ANDROID)
  list(APPEND SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_surface_android.cpp
  )
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
  m
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
