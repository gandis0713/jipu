#pragma once

#include "vulkan_api.h"

namespace vkt
{

struct VulkanHeapMemoryDescriptor
{
    VkMemoryPropertyFlags flags{};
    VkMemoryRequirements requirements{};
};

class VulkanDevice;
class VulkanHeapMemory
{
public:
    VulkanHeapMemory() = delete;
    VulkanHeapMemory(VulkanDevice* device, const VulkanHeapMemoryDescriptor& descriptor) noexcept(false);
    ~VulkanHeapMemory();

    VkDeviceMemory getVkDeviceMemory() const;

private:
    VulkanDevice* m_device{ nullptr };
    VkDeviceMemory m_deviceMemory{ VK_NULL_HANDLE };
};

} // namespace vkt