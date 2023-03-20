#include "vulkan_adapter.h"
#include "vulkan_device.h"
#include "vulkan_driver.h"

#if defined(__linux__)
#elif defined(_WIN64)
    #include "vulkan_platform_windows.h"
#elif defined(__APPLE__)
    #include "vulkan_platform_macos.h"
#endif

namespace vkt
{

VulkanAdapter::VulkanAdapter(VulkanDriver* vulkanDriver, AdapterCreateInfo info)
    : Adapter(vulkanDriver, info)
{
    m_physicalDevice = vulkanDriver->getPhysicalDevices()[0];
}

std::unique_ptr<Device> VulkanAdapter::createDevice(DeviceCreateInfo info)
{
    return std::make_unique<VulkanDevice>(this, info);
}

std::unique_ptr<Platform> VulkanAdapter::createPlatform(PlatformCreateInfo info)
{
#if defined(__linux__)
    return nullptr;
#elif defined(_WIN64)
    return std::make_unique<VulkanPlatformWindows>(this, info);
#elif defined(__APPLE__)
    return std::make_unique<VulkanPlatformMacOS>(this, info);
#endif
}

VkInstance VulkanAdapter::getInstance() const
{
    return static_cast<VulkanDriver*>(m_driver)->getInstance();
}

VkPhysicalDevice VulkanAdapter::getPhysicalDevice() const
{
    return m_physicalDevice;
}

} // namespace vkt
