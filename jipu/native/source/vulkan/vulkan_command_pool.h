#pragma once

#include "vulkan_api.h"

#include <unordered_map>

namespace jipu
{

struct VulkanCommandPoolDescriptor
{
};

class VulkanDevice;
class VulkanCommandPool final
{
public:
    VulkanCommandPool() = delete;
    explicit VulkanCommandPool(VulkanDevice* device, const VulkanCommandPoolDescriptor& descriptor);
    ~VulkanCommandPool();

public:
    VkCommandBuffer create(/* TODO */);
    void release(VkCommandBuffer commandBuffer);

private:
    VulkanDevice* m_device = nullptr;

private:
    VkCommandPool m_commandPool = VK_NULL_HANDLE;

private:
    std::unordered_map<VkCommandBuffer, bool> m_commandBuffers{};
};

} // namespace jipu