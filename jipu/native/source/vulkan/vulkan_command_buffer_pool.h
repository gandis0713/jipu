#pragma once

#include "vulkan_api.h"

#include <unordered_map>

namespace jipu
{

class VulkanDevice;
class VulkanCommandBufferPool final
{
public:
    VulkanCommandBufferPool() = delete;
    explicit VulkanCommandBufferPool(VulkanDevice* device);
    ~VulkanCommandBufferPool();

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