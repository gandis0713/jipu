#pragma once

#include "vulkan.h"

namespace vkt
{

struct Context
{
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
};

} // namespace vkt