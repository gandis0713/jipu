#pragma once

#include "vk/vulkan_api.h"

namespace vkt
{

struct Context
{
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
};

} // namespace vkt