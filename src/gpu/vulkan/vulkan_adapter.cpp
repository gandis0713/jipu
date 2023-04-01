#include "vulkan_adapter.h"
#include "vulkan_device.h"
#include "vulkan_driver.h"

#include "utils/log.h"

#if defined(__linux__)
#elif defined(_WIN64)
    #include "vulkan_platform_windows.h"
#elif defined(__APPLE__)
    #include "vulkan_platform_macos.h"
#endif

namespace vkt
{

VulkanAdapter::VulkanAdapter(VulkanDriver* vulkanDriver, AdapterDescriptor descriptor)
    : Adapter(vulkanDriver, descriptor)
{
    m_physicalDevice = vulkanDriver->getPhysicalDevices()[0];
}

VulkanAdapter::~VulkanAdapter()
{
    // doesn't need to desotyr VkPhysicalDevice.
    LOG_TRACE(__func__);
}

std::unique_ptr<Device> VulkanAdapter::createDevice(DeviceDescriptor descriptor)
{
    return std::make_unique<VulkanDevice>(this, descriptor);
}

std::unique_ptr<Platform> VulkanAdapter::createPlatform(PlatformDescriptor descriptor)
{
#if defined(__linux__)
    return nullptr;
#elif defined(_WIN64)
    return std::make_unique<VulkanPlatformWindows>(this, descriptor);
#elif defined(__APPLE__)
    return std::make_unique<VulkanPlatformMacOS>(this, descriptor);
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
