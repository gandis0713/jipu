#pragma once

#include "jipu/gpu/queue.h"
#include "utils/cast.h"
#include "vulkan_api.h"

namespace jipu
{

class VulkanDevice;
class VulkanQueue : public Queue
{
public:
    VulkanQueue() = delete;
    VulkanQueue(VulkanDevice* device, const QueueDescriptor& descriptor) noexcept(false);
    ~VulkanQueue() override;

    void submit(std::vector<CommandBuffer*> commandBuffers) override;
    void submit(std::vector<CommandBuffer*> commandBuffers, Swapchain* swapchain) override;

public:
    VkQueue getVkQueue() const;

private:
    VkQueue m_queue = VK_NULL_HANDLE;
    VkFence m_fence = VK_NULL_HANDLE;

    // TODO: use pair.
    uint32_t m_index{ 0 }; // Index in VkQueueFamilyProperties in VkPhysicalDevice
    VkQueueFamilyProperties m_properties{};

private:
    struct SubmitInfo
    {
        VkCommandBuffer cmdBuf = VK_NULL_HANDLE;
        std::pair<std::vector<VkSemaphore>, std::vector<VkPipelineStageFlags>> signal{};
        std::pair<std::vector<VkSemaphore>, std::vector<VkPipelineStageFlags>> wait{};

    } m_submitInfo;

    std::vector<SubmitInfo> gatherSubmitInfo(std::vector<CommandBuffer*> commandBuffers);
    void submit(const std::vector<SubmitInfo>& submitInfos);
};

DOWN_CAST(VulkanQueue, Queue);

// Convert Helper
VkQueueFlags ToVkQueueFlags(QueueFlags flags);
QueueFlags ToQueueFlags(VkQueueFlags vkflags);

} // namespace jipu