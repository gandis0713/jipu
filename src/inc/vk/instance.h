#pragma once

#include <vector>
#include <vulkan/vulkan.h>

#include "allocation_callbacks.h"

namespace vkt {

class Instance {
public:
  Instance(const std::vector<const char *> &instance_extensions,
           const std::vector<const char *> &layers,
           uint32_t vulkan_api_version = VK_API_VERSION_1_2,
           AllocationCallbacks *allocator = nullptr);
};

} // namespace vkt