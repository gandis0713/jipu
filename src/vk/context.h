#pragma once

#include "vk/vulkan_api.h"
#include <optional>

namespace vkt
{

struct QueueFamilyIndices
{
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily; // It should be same with graphics family? ref: https://github.com/google/filament/issues/1532

    bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }

    static QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& physicalDevice);
};

struct Context
{
    void initialize();
    void finalize();

    VkInstance instance{};             // single instance.
    VkPhysicalDevice physicalDevice{}; // single physical devcie.
    VkDevice device{};
    VkQueue graphicsQueue;
    VkQueue presentQueue;
};

} // namespace vkt