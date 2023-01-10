#pragma once

#include "vk/visibility.h"
#include "vk/vulkan.h"

namespace vkt
{

class VKT_EXPORT Context
{
public:
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
};

} // namespace vkt