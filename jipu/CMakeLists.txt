cmake_minimum_required(VERSION 3.22)

# find package
find_package(VulkanHeaders CONFIG)
find_package(VulkanMemoryAllocator CONFIG REQUIRED)
find_library(VulkanSDK NAMES vulkan HINTS "$ENV{VULKAN_SDK}/lib")
find_package(spdlog CONFIG REQUIRED)

set(SRC_FILES
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_api.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_binding_group_layout.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_binding_group.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_buffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_command_buffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_command_encoder.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_compute_pass_encoder.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_device.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_driver.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_framebuffer.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_resource_allocator.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_pipeline.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_pipeline_layout.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_physical_device.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_queue.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_render_pass.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_render_pass_encoder.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_shader_module.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_sampler.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_surface.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_swapchain.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_texture.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_texture_view.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_api.h
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_binding_group_layout.h
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_binding_group.h
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_buffer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_command_buffer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_command_encoder.h
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_compute_pass_encoder.h
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_device.h
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_driver.h
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_framebuffer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_resource.h
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_resource_allocator.h
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_pipeline.h
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_pipeline_layout.h
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_physical_device.h
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_queue.h
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_render_pass.h
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_render_pass_encoder.h
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_sampler.h
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_shader_module.h
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_surface.h
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_swapchain.h
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_texture.h
  ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_texture_view.h

  ${CMAKE_CURRENT_SOURCE_DIR}/source/jipu/driver.cpp

  ${CMAKE_CURRENT_SOURCE_DIR}/include/jipu/binding_group.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jipu/pipeline_layout.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jipu/command_buffer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jipu/command_encoder.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jipu/compute_pass_encoder.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jipu/texture.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jipu/device.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jipu/pipeline.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jipu/buffer.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jipu/swapchain.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jipu/driver.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jipu/queue.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jipu/render_pass_encoder.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jipu/texture_view.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jipu/physical_device.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jipu/sampler.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jipu/surface.h
  ${CMAKE_CURRENT_SOURCE_DIR}/include/jipu/shader_module.h
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
    ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_surface_macos.mm
  )
elseif(MSVC)
  list(APPEND SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_surface_window.cpp
  )
elseif(ANDROID)
  list(APPEND SRC_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan/vulkan_surface_android.cpp
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
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
  PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

if(ENABLE_EXPERIMENTAL)
  target_include_directories(jipu
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/source/vulkan>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  )
endif()

# link libraries
target_link_libraries(jipu
  PRIVATE
  Vulkan::Headers
  GPUOpen::VulkanMemoryAllocator
  ${VulkanSDK}
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

if(BUILD_SHARED_LIBS)
  target_compile_definitions(jipu
    PRIVATE
    "JIPU_SHARED_LIBRARY"
    "JIPU_IMPLEMENTATION"
  )

  if(ENABLE_EXPERIMENTAL)
    target_compile_definitions(jipu
      PRIVATE
      "EXPERIMENTAL"
    )
  endif()
endif()

if(MSVC)
  target_compile_definitions(jipu
    PRIVATE
    "NOMINMAX" # to undefine max, min macro
  )
endif()

install(TARGETS jipu
  EXPORT ${PROJECT_NAME}-config
  RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin
  LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
  ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include
  DESTINATION ${CMAKE_INSTALL_PREFIX})
