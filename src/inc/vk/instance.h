#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "allocation_callbacks.h"

namespace vks 
{

  class Instance
  {
  public:
    Instance(const std::vector<const char*>& instance_extensions,
              const std::vector<const char*>& layers,
              uint32_t vulkan_api_version = VK_API_VERSION_1_2,
              AllocationCallbacks* allocator = nullptr);
  };

} // namespace vks