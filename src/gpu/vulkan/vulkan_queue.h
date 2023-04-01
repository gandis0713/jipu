#pragma once

#include "gpu/queue.h"
#include "vulkan_api.h"

namespace vkt
{

class VulkanDevice;
class VulkanQueue : public Queue
{
public:
    VulkanQueue() = delete;
    VulkanQueue(VulkanDevice* device, const QueueDescriptor& descriptor);
    ~VulkanQueue() override;

    VkQueue getQueue() const;

    QueueType getType() const override;

private:
    VkQueue m_queue{ VK_NULL_HANDLE };

    // TODO: use pair.
    uint32_t m_index{ 0 }; // Index in VkQueueFamilyProperties in VkPhysicalDevice
    VkQueueFamilyProperties m_properties{};
};

} // namespace vkt