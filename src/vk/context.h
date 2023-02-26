#pragma once

#include "vk/precompile.h"

namespace vkt
{

struct Context
{
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
};

} // namespace vkt