#include "vulkan_queue.h"
#include "vulkan_device.h"
#include "vulkan_physical_device.h"

#include "utils/log.h"

#include <stdexcept>

namespace vkt
{

VulkanQueue::VulkanQueue(VulkanDevice* device, const QueueDescriptor& descriptor) noexcept(false)
    : Queue(device, descriptor)
{
    VulkanPhysicalDevice* physicalDevice = downcast(m_device->getPhysicalDevice());

    const VulkanPhysicalDeviceInfo& deviceInfo = physicalDevice->getInfo();

    const uint32_t queueFamilyPropertiesSize = deviceInfo.queueFamilyProperties.size();
    if (queueFamilyPropertiesSize <= 0)
    {
        throw std::runtime_error("There is no queue family properties.");
    }

    for (uint32_t index = 0; index < queueFamilyPropertiesSize; ++index)
    {
        const auto& properties = deviceInfo.queueFamilyProperties[index];
        if (properties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            m_index = index;
            m_properties = properties;
            break;
        }
    }

    device->vkAPI.GetDeviceQueue(device->getVkDevice(), m_index, 0, &m_queue);
}

VulkanQueue::~VulkanQueue()
{
    // Doesn't need to destroy VkQueue.
}

void VulkanQueue::submit()
{
}

} // namespace vkt
