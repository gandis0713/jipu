#include "vulkan_queue.h"
#include "vulkan_adapter.h"
#include "vulkan_device.h"

#include "utils/log.h"

#include <stdexcept>

namespace vkt
{

VulkanQueue::VulkanQueue(VulkanDevice* device, const QueueDescriptor& descriptor) noexcept(false)
    : Queue(device, descriptor)
{
    VulkanAdapter* adapter = static_cast<VulkanAdapter*>(m_device->getAdapter());

    const VulkanDeviceInfo& deviceInfo = adapter->getDeviceInfo();

    const uint32_t queueFamilyPropertiesSize = deviceInfo.queueFamilyProperties.size();
    if (queueFamilyPropertiesSize <= 0)
    {
        throw std::runtime_error("There is no queue family properties.");
    }

    for (auto index = 0; index < queueFamilyPropertiesSize; ++index)
    {
        const auto& properties = deviceInfo.queueFamilyProperties[index];
        if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            m_index = index;
            m_properties = properties;
            break;
        }
    }

    device->vkAPI.GetDeviceQueue(device->getDevice(), m_index, 0, &m_queue);
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
