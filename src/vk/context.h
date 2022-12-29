#pragma once

#include "vulkan.h"

namespace vk
{

struct Context
{
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
};

} // namespace vk