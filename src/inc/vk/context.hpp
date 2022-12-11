#pragma once

#include <vulkan/vulkan.h>

namespace vk 
{

struct Context 
{
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
};

}