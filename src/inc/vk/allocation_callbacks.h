#pragma once

#include <vulkan/vulkan.h>

namespace vkt {

class AllocationCallbacks : public VkAllocationCallbacks {
public:
  AllocationCallbacks() : VkAllocationCallbacks{} {}

protected:
  virtual ~AllocationCallbacks() {}
};

} // namespace vkt