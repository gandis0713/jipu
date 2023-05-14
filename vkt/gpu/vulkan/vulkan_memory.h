#pragma once

#include "vulkan_api.h"

namespace vkt
{

struct VulkanMemoryDescriptor
{
    VkMemoryPropertyFlags flags{};
    VkMemoryRequirements requirements{};
};

class VulkanDevice;
class VulkanMemory
{
public:
    VulkanMemory() = delete;
    VulkanMemory(VulkanDevice* device, const VulkanMemoryDescriptor& descriptor) noexcept(false);
    ~VulkanMemory();

    VkDeviceMemory getVkDeviceMemory() const;

private:
    VulkanDevice* m_device{ nullptr };
    VkDeviceMemory m_deviceMemory{ VK_NULL_HANDLE };
};

} // namespace vkt