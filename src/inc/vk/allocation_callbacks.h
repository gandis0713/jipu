#pragma once 

#include <vulkan/vulkan.h>

namespace vks 
{

  class AllocationCallbacks : public VkAllocationCallbacks
  {
    public:
        AllocationCallbacks() :
            VkAllocationCallbacks{} {}

    protected:
        virtual ~AllocationCallbacks() {}
  };

} // namespace vks