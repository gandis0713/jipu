#include "vulkan_queue.h"
#include "vulkan_device.h"

#include "utils/log.h"

namespace vkt
{

VulkanQueue::VulkanQueue(VulkanDevice* device, const QueueDescriptor& descriptor)
    : Queue(device, descriptor)
{
}

VulkanQueue::~VulkanQueue()
{
    // Doesn't need to destroy VkQueue.
    LOG_TRACE(__func__);
}

} // namespace vkt