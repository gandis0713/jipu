#include "vulkan_adapter.h"
#include "vulkan_device.h"
#include "vulkan_driver.h"

#include "utils/log.h"

namespace vkt
{

VulkanAdapter::VulkanAdapter(VulkanDriver* vulkanDriver, AdapterDescriptor descriptor)
    : Adapter(vulkanDriver, descriptor)
{
    m_physicalDevice = vulkanDriver->getPhysicalDevice(descriptor.index);

    // Gather device information.
    gatherDeviceInfo();
}

VulkanAdapter::~VulkanAdapter()
{
    // doesn't need to destroy VkPhysicalDevice.
    LOG_TRACE(__func__);
}

std::unique_ptr<Device> VulkanAdapter::createDevice(DeviceDescriptor descriptor)
{
    return std::make_unique<VulkanDevice>(this, descriptor);
}

VkInstance VulkanAdapter::getInstance() const
{
    return downcast(m_driver)->getInstance();
}

VkPhysicalDevice VulkanAdapter::getPhysicalDevice() const
{
    return m_physicalDevice;
}

const VulkanDeviceInfo& VulkanAdapter::getDeviceInfo() const
{
    return m_deviceInfo;
}

void VulkanAdapter::gatherDeviceInfo()
{
    const VulkanAPI& vkAPI = downcast(m_driver)->vkAPI;

    // Gather physical device properties and features.
    vkAPI.GetPhysicalDeviceProperties(m_physicalDevice, &m_deviceInfo.physicalDeviceProperties);
    vkAPI.GetPhysicalDeviceFeatures(m_physicalDevice, &m_deviceInfo.physicalDeviceFeatures);

    // Gather queue Family Properties.
    {
        uint32_t queueFamilyCount = 0;
        vkAPI.GetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, nullptr);

        m_deviceInfo.queueFamilyProperties.resize(queueFamilyCount);
        vkAPI.GetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, m_deviceInfo.queueFamilyProperties.data());
    }

    // Gather device layer properties.
    {
        uint32_t deviceLayerCount = 0;
        vkAPI.EnumerateDeviceLayerProperties(m_physicalDevice, &deviceLayerCount, nullptr);

        m_deviceInfo.layerProperties.resize(deviceLayerCount);
        vkAPI.EnumerateDeviceLayerProperties(m_physicalDevice, &deviceLayerCount, m_deviceInfo.layerProperties.data());
    }

    // Gather device extension properties.
    {
        uint32_t deviceExtensionCount = 0;
        vkAPI.EnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &deviceExtensionCount, nullptr);

        m_deviceInfo.extensionProperties.resize(deviceExtensionCount);
        vkAPI.EnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &deviceExtensionCount, m_deviceInfo.extensionProperties.data());
    }
}

} // namespace vkt
