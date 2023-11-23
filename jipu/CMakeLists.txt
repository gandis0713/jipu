cmake_minimum_required(VERSION 3.12)

# find package
find_package(Vulkan REQUIRED)
find_package(spdlog CONFIG REQUIRED)

set(SRC_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_api.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_binding_group_layout.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_binding_group.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_buffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_command_buffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_command_encoder.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_compute_pass_encoder.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_device.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_driver.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_framebuffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_memory.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_memory_allocator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_pipeline.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_pipeline_layout.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_physical_device.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_queue.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_render_pass.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_render_pass_encoder.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_shader_module.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_sampler.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_surface.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_swapchain.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_texture.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_texture_view.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_api.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_binding_group_layout.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_binding_group.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_buffer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_command_buffer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_command_encoder.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_compute_pass_encoder.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_device.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_driver.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_framebuffer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_memory.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_memory_allocator.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_pipeline.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_pipeline_layout.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_physical_device.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_queue.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_render_pass.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_render_pass_encoder.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_sampler.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_shader_module.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_surface.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_swapchain.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_texture.h
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/vulkan/vulkan_texture_view.h

  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/binding_group_layout.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/binding_group.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/pipeline_layout.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/command_encoder.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/command_buffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/compute_pass_encoder.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/texture.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/device.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/pipeline.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/buffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/swapchain.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/driver.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/queue.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/render_pass_encoder.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/texture_view.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/physical_device.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/sampler.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/surface.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/gpu/shader_module.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/include/jipu/gpu/binding_group.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jipu/gpu/pipeline_layout.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jipu/gpu/command_buffer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jipu/gpu/command_encoder.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jipu/gpu/compute_pass_encoder.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jipu/gpu/texture.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jipu/gpu/device.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jipu/gpu/pipeline.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jipu/gpu/buffer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jipu/gpu/swapchain.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jipu/gpu/driver.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jipu/gpu/queue.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jipu/gpu/render_pass_encoder.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jipu/gpu/texture_view.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jipu/gpu/physical_device.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jipu/gpu/sampler.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jipu/gpu/surface.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jipu/gpu/shader_module.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/export.h

  ${CMAKE_CURRENT_SOURCE_DIR}/utils/dylib.cpp

  # ${CMAKE_CURRENT_SOURCE_DIR}/utils/dylib.h
  # ${CMAKE_CURRENT_SOURCE_DIR}/utils/fmt.h
  # ${CMAKE_CURRENT_SOURCE_DIR}/utils/cast.h
  # ${CMAKE_CURRENT_SOURCE_DIR}/utils/assert.h
  # ${CMAKE_CURRENT_SOURCE_DIR}/utils/hash.h
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

add_library(jipu
  ${LIB_TYPE} # SHARED or STATIC
  ${SRC_FILES}
)
add_library(jipu::jipu ALIAS jipu)

# include directories
target_include_directories(jipu
  PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/include # for export
  PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}
)

# link libraries
target_link_libraries(jipu
  PUBLIC
  Vulkan::Vulkan
  PRIVATE
  spdlog::spdlog_header_only
)

if(APPLE)
  target_link_libraries(jipu
    PRIVATE
    "-framework Cocoa"
  )
endif()

# compile options
if(MSVC)
# TODO
elseif(APPLE)
  target_compile_options(jipu
    PUBLIC
    -Wall
    -Werror
    -Wshorten-64-to-32
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
target_compile_definitions(jipu
  PRIVATE
  "VKT_IMPLEMENTATION"
)

if(BUILD_SHARED_LIBS)
  target_compile_definitions(jipu
    PRIVATE
    "VKT_SHARED_LIBRARY"
  )
endif()

if(MSVC)
  target_compile_definitions(jipu
    PRIVATE
    "NOMINMAX" # to undefine max, min macro
  )
endif()

# install
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include DESTINATION ${CMAKE_INSTALL_PREFIX})

install(TARGETS jipu
  INCLUDES DESTINATION ${CMAKE_INSTALL_PREFIX}/include
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
  LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
  ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)