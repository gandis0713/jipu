#pragma once

#include <vulkan/vulkan.h>
#include "allocation_callbacks.h"

namespace vks 
{

  class Instance
  {
  public:
    Instance(const char** instance_extensions,
              const char** layers,
              uint32_t vulkan_version = VK_API_VERSION_1_2,
              AllocationCallbacks* allocator = nullptr);
  };

} // namespace vks