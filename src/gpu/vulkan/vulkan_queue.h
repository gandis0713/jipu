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

private:
    VkQueue m_queue;
};

} // namespace vkt