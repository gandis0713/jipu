#include "vulkan_queue.h"
#include "vulkan_adapter.h"
#include "vulkan_device.h"

#include "utils/log.h"

namespace vkt
{

VulkanQueue::VulkanQueue(VulkanDevice* device, const QueueDescriptor& descriptor)
    : Queue(device, descriptor)
{
    VulkanAdapter* adapter = static_cast<VulkanAdapter*>(m_device->getAdapter());

    const VulkanDeviceInfo& deviceInfo = adapter->getDeviceInfo();

    for (auto index = 0; index < deviceInfo.queueFamilyProperties.size(); ++index)
    {
        const auto& properties = deviceInfo.queueFamilyProperties[index];
        if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            m_index = index;
            m_properties = properties;
            break;
        }
    }

    vkGetDeviceQueue(device->getDevice(), m_index, 0, &m_queue);
}

VulkanQueue::~VulkanQueue()
{
    // Doesn't need to destroy VkQueue.
    LOG_TRACE(__func__);
}

VkQueue VulkanQueue::getQueue() const
{
    return m_queue;
}

QueueType VulkanQueue::getType() const
{
    return QueueType::kGraphics; // TODO: use m_properties;
}

} // namespace vkt
