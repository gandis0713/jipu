cmake_minimum_required(VERSION 3.22)

# find header library
find_path(WEBGPU_HEADERS_INCLUDE_DIRS "webgpu.h")

# find package
find_package(VulkanHeaders CONFIG)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)

# find_library(VulkanSDK NAMES vulkan HINTS "$ENV{VULKAN_SDK}/lib")
find_package(spdlog CONFIG REQUIRED)

set(SRC_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_api.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_binding_group_layout.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_binding_group.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_buffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_command_buffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_command_encoder.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_compute_pass_encoder.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_device.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_instance.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_framebuffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_resource_allocator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_pipeline.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_pipeline_layout.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_physical_device.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_query_set.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_queue.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_render_pass.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_render_pass_encoder.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_shader_module.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_sampler.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_surface.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_swapchain.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_texture.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_texture_view.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_api.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_binding_group_layout.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_binding_group.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_buffer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_command_buffer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_command_encoder.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_compute_pass_encoder.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_device.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_instance.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_framebuffer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_resource.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_resource_allocator.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_pipeline.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_pipeline_layout.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_physical_device.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_query_set.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_queue.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_render_pass.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_render_pass_encoder.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_sampler.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_shader_module.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_surface.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_swapchain.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_texture.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_texture_view.h

  ${CMAKE_CURRENT_SOURCE_DIR}/native/source/jipu/instance.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/native/include/jipu/binding_group.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/include/jipu/pipeline_layout.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/include/jipu/command_buffer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/include/jipu/command_encoder.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/include/jipu/compute_pass_encoder.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/include/jipu/texture.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/include/jipu/device.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/include/jipu/pipeline.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/include/jipu/query_set.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/include/jipu/buffer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/include/jipu/swapchain.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/include/jipu/instance.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/include/jipu/queue.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/include/jipu/render_pass_encoder.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/include/jipu/texture_view.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/include/jipu/physical_device.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/include/jipu/sampler.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/include/jipu/surface.h
  ${CMAKE_CURRENT_SOURCE_DIR}/native/include/jipu/shader_module.h

  ${CMAKE_CURRENT_SOURCE_DIR}/native/include/export.h

  ${CMAKE_CURRENT_SOURCE_DIR}/common/dylib.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/common/gpu_info.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/common/ref_counted.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/common/dylib.h
  ${CMAKE_CURRENT_SOURCE_DIR}/common/gpu_info.h
  ${CMAKE_CURRENT_SOURCE_DIR}/common/ref_counted.h

  ${CMAKE_CURRENT_SOURCE_DIR}/common/result.h

  ${CMAKE_CURRENT_SOURCE_DIR}/webgpu/webgpu_adapter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/webgpu/webgpu_adapter.h
  ${CMAKE_CURRENT_SOURCE_DIR}/webgpu/webgpu_bind_group_layout.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/webgpu/webgpu_bind_group_layout.h
  ${CMAKE_CURRENT_SOURCE_DIR}/webgpu/webgpu_bind_group.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/webgpu/webgpu_bind_group.h
  ${CMAKE_CURRENT_SOURCE_DIR}/webgpu/webgpu_buffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/webgpu/webgpu_buffer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/webgpu/webgpu_device.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/webgpu/webgpu_device.h
  ${CMAKE_CURRENT_SOURCE_DIR}/webgpu/webgpu_instance.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/webgpu/webgpu_instance.h
  ${CMAKE_CURRENT_SOURCE_DIR}/webgpu/webgpu_pipeline_layout.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/webgpu/webgpu_pipeline_layout.h
  ${CMAKE_CURRENT_SOURCE_DIR}/webgpu/webgpu_queue.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/webgpu/webgpu_queue.h
  ${CMAKE_CURRENT_SOURCE_DIR}/webgpu/webgpu_render_pipeline.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/webgpu/webgpu_render_pipeline.h
  ${CMAKE_CURRENT_SOURCE_DIR}/webgpu/webgpu_sampler.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/webgpu/webgpu_sampler.h
  ${CMAKE_CURRENT_SOURCE_DIR}/webgpu/webgpu_surface.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/webgpu/webgpu_surface.h
  ${CMAKE_CURRENT_SOURCE_DIR}/webgpu/webgpu_texture.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/webgpu/webgpu_texture.h

  # ${CMAKE_CURRENT_SOURCE_DIR}/common/fmt.h
  # ${CMAKE_CURRENT_SOURCE_DIR}/common/cast.h
  # ${CMAKE_CURRENT_SOURCE_DIR}/common/assert.h
  # ${CMAKE_CURRENT_SOURCE_DIR}/common/hash.h
  ${CMAKE_CURRENT_SOURCE_DIR}/jipu_proc.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/webgpu_proc.cpp
)

if(APPLE)
  list(APPEND SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_surface_macos.mm
  )
elseif(MSVC)
  list(APPEND SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_surface_window.cpp
  )
elseif(ANDROID)
  list(APPEND SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan/vulkan_surface_android.cpp
  )
endif()

set(LIB_TYPE STATIC)

if(BUILD_SHARED_LIBS)
  set(LIB_TYPE SHARED)
endif()

add_library(jipu_native
  ${LIB_TYPE} # SHARED or STATIC
  ${SRC_FILES}
)
add_library(jipu::native ALIAS jipu_native)

# include directories
target_include_directories(jipu_native
  PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/native/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/native/include>
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<BUILD_INTERFACE:${WEBGPU_HEADERS_INCLUDE_DIRS}>
)

if(EXPORT_JIPU_NATIVE)
  target_include_directories(jipu_native
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/native/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/native/include>
  )
endif()

if(EXPORT_JIPU_VULKAN)
  target_include_directories(jipu_native
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/native/source/vulkan>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  )
endif()

# link libraries
target_link_libraries(jipu_native
  PRIVATE
  Vulkan::Headers
  GPUOpen::VulkanMemoryAllocator

  # ${VulkanSDK}
  spdlog::spdlog_header_only
)

if(APPLE)
  target_link_libraries(jipu_native
    PRIVATE
    "-framework Cocoa"
  )
endif()

# compile options
if(MSVC)
# TODO
elseif(APPLE)
  target_compile_options(jipu_native
    PRIVATE
    -Wall
    -Werror
    -Wshorten-64-to-32
    -Wno-unused-parameter
    -Wno-unused-function
    -Wno-unused-but-set-variable
    -Wno-unused-variable

    -fvisibility=hidden
    -fvisibility-inlines-hidden

    -fno-rtti # -fno-exceptions
  )
endif()

target_compile_definitions(jipu_native
  PRIVATE
  "JIPU_IMPLEMENTATION"
  "WGPU_IMPLEMENTATION"
)

if(USE_DAWN_WEBGPU)
  target_compile_definitions(jipu_native
    PRIVATE
    "USE_DAWN_HEADER"
  )
endif()

if(EXPORT_JIPU_VULKAN)
  target_compile_definitions(jipu_native
    PRIVATE
    "VULKAN_IMPLEMENTATION"
  )
endif()

if(BUILD_SHARED_LIBS)
  target_compile_definitions(jipu_native
    PRIVATE
    "JIPU_SHARED_LIBRARY"
    "WGPU_SHARED_LIBRARY"
  )
endif()

if(MSVC)
  target_compile_definitions(jipu_native
    PRIVATE
    "NOMINMAX" # to undefine max, min macro
  )
endif()

install(TARGETS jipu_native
  EXPORT ${PROJECT_NAME}-config
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
  LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
  ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)

if(EXPORT_JIPU_NATIVE)
  install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
    DESTINATION ${CMAKE_INSTALL_PREFIX})
endif()
