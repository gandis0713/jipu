#include "vulkan_physical_device.h"
#include "vulkan_device.h"
#include "vulkan_driver.h"

#include "utils/log.h"
#include <fmt/format.h>
#include <stdexcept>

namespace vkt
{

VulkanPhysicalDevice::VulkanPhysicalDevice(VulkanDriver* vulkanDriver, PhysicalDeviceDescriptor descriptor)
    : PhysicalDevice(vulkanDriver, descriptor)
{
    m_physicalDevice = vulkanDriver->getVkPhysicalDevice(descriptor.index);

    // Gather device information.
    gatherPhysicalDeviceInfo();
}

VulkanPhysicalDevice::~VulkanPhysicalDevice()
{
    // doesn't need to destroy VkPhysicalDevice.
}

std::unique_ptr<Device> VulkanPhysicalDevice::createDevice(DeviceDescriptor descriptor)
{
    return std::make_unique<VulkanDevice>(this, descriptor);
}

VkInstance VulkanPhysicalDevice::getVkInstance() const
{
    return downcast(m_driver)->getVkInstance();
}

VkPhysicalDevice VulkanPhysicalDevice::getVkPhysicalDevice() const
{
    return m_physicalDevice;
}

const VulkanPhysicalDeviceInfo& VulkanPhysicalDevice::getInfo() const
{
    return m_Info;
}

void VulkanPhysicalDevice::gatherPhysicalDeviceInfo()
{
    const VulkanAPI& vkAPI = downcast(m_driver)->vkAPI;

    // Gather physical device properties and features.
    vkAPI.GetPhysicalDeviceProperties(m_physicalDevice, &m_Info.physicalDeviceProperties);
    vkAPI.GetPhysicalDeviceFeatures(m_physicalDevice, &m_Info.physicalDeviceFeatures);

    // Gather device memory properties.
    {
        VkPhysicalDeviceMemoryProperties memoryProperties{};
        vkAPI.GetPhysicalDeviceMemoryProperties(m_physicalDevice, &memoryProperties);

        m_Info.memoryTypes.assign(memoryProperties.memoryTypes, memoryProperties.memoryTypes + memoryProperties.memoryTypeCount);
        m_Info.memoryHeaps.assign(memoryProperties.memoryHeaps, memoryProperties.memoryHeaps + memoryProperties.memoryHeapCount);
    }

    // Gather queue Family Properties.
    {
        uint32_t queueFamilyCount = 0;
        vkAPI.GetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, nullptr);

        m_Info.queueFamilyProperties.resize(queueFamilyCount);
        vkAPI.GetPhysicalDeviceQueueFamilyProperties(m_physicalDevice, &queueFamilyCount, m_Info.queueFamilyProperties.data());
    }

    // Gather device layer properties.
    {
        uint32_t deviceLayerCount = 0;
        VkResult result = vkAPI.EnumerateDeviceLayerProperties(m_physicalDevice, &deviceLayerCount, nullptr);
        if (result != VK_SUCCESS && result != VK_INCOMPLETE)
        {
            throw std::runtime_error(fmt::format("Failure EnumerateDeviceLayerProperties to get count. Error: {}", result));
        }

        m_Info.layerProperties.resize(deviceLayerCount);
        result = vkAPI.EnumerateDeviceLayerProperties(m_physicalDevice, &deviceLayerCount, m_Info.layerProperties.data());
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error(fmt::format("Failure EnumerateDeviceLayerProperties. Error: {}", result));
        }

        for (const auto& layerProperty : m_Info.layerProperties)
        {
            LOG_INFO("Device Layer Name: {}", layerProperty.layerName);
        }
    }

    // Gather device extension properties.
    {
        uint32_t deviceExtensionCount = 0;
        VkResult result = vkAPI.EnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &deviceExtensionCount, nullptr);
        if (result != VK_SUCCESS && result != VK_INCOMPLETE)
        {
            throw std::runtime_error(fmt::format("Failure EnumerateDeviceExtensionProperties to get count. Error: {}", result));
        }

        m_Info.extensionProperties.resize(deviceExtensionCount);
        result = vkAPI.EnumerateDeviceExtensionProperties(m_physicalDevice, nullptr, &deviceExtensionCount, m_Info.extensionProperties.data());
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error(fmt::format("Failure EnumerateDeviceExtensionProperties. Error: {}", result));
        }

        for (const auto& extensionProperty : m_Info.extensionProperties)
        {
            LOG_INFO("Device Extention Name: {}", extensionProperty.extensionName);

            // TODO: define "VK_KHR_portability_subset"
            if (strncmp(extensionProperty.extensionName, "VK_KHR_portability_subset", VK_MAX_EXTENSION_NAME_SIZE) == 0)
            {
                m_Info.portabilitySubset = true;
            }
        }
    }
}

int VulkanPhysicalDevice::fineMemoryTypeIndex(VkMemoryPropertyFlags flags) const
{
    int memoryTypeIndex = -1;
    for (int i = 0u; i < m_Info.memoryTypes.size(); ++i)
    {
        const auto& memoryType = m_Info.memoryTypes[i];
        if ((memoryType.propertyFlags & flags) == flags)
        {
            memoryTypeIndex = i;
            break;
        }
    }

    return memoryTypeIndex;
}

} // namespace vkt
