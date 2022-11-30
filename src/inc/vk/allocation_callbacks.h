#pragma once

#include <vulkan/vulkan.h>

namespace vk {

class AllocationCallbacks : public VkAllocationCallbacks {
public:
  AllocationCallbacks() : VkAllocationCallbacks{} {}

protected:
  virtual ~AllocationCallbacks() {}
};

} // namespace vk