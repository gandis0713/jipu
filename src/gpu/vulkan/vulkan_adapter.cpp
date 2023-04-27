#include "vulkan_adapter.h"
#include "vulkan_device.h"
#include "vulkan_driver.h"
#include "vulkan_surface.h"

#include "utils/assert.h"
#include "utils/log.h"

namespace vkt
{

VulkanAdapter::VulkanAdapter(VulkanDriver* vulkanDriver, AdapterDescriptor descriptor)
    : Adapter(vulkanDriver, descriptor)
{
    const std::vector<VkPhysicalDevice>& physicalDevices = vulkanDriver->getPhysicalDevices();
    assert(descriptor.index < physicalDevices.size());
    m_physicalDevice = physicalDevices[descriptor.index];

    // Gather device information.
    gatherDeviceInfo();

    // Gather sruface information.
    gatherSurfaceInfo();
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

std::unique_ptr<Surface> VulkanAdapter::createSurface(SurfaceDescriptor descriptor)
{
    return std::make_unique<VulkanSurface>(this, descriptor);
}

VkInstance VulkanAdapter::getInstance() const
{
    return static_cast<VulkanDriver*>(m_driver)->getInstance();
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
    const VulkanAPI& vkAPI = static_cast<VulkanDriver*>(m_driver)->getAPI();

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

void VulkanAdapter::gatherSurfaceInfo()
{
}

} // namespace vkt
