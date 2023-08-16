#pragma once

#include "utils/cast.h"
#include "vkt/gpu/queue.h"
#include "vulkan_api.h"

namespace vkt
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
    VkSemaphore m_renderSemaphore = VK_NULL_HANDLE;
    VkFence m_fence = VK_NULL_HANDLE;

    // TODO: use pair.
    uint32_t m_index{ 0 }; // Index in VkQueueFamilyProperties in VkPhysicalDevice
    VkQueueFamilyProperties m_properties{};
};

DOWN_CAST(VulkanQueue, Queue);

// Convert Helper
VkQueueFlags ToVkQueueFlags(QueueFlags flags);
QueueFlags ToQueueFlags(VkQueueFlags vkflags);

} // namespace vkt