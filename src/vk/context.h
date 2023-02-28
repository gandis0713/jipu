#pragma once

#include "vk/vulkan_api.h"

namespace vkt
{

struct Context
{
    void initialize();
    void finalize();

    VkInstance instance{};
    VkPhysicalDevice physicalDevice{};
    VkDevice device{};
};

} // namespace vkt