#include "vulkan_surface.h"
#include "vulkan_adapter.h"
#include "vulkan_driver.h"

namespace vkt
{

VulkanSurface::VulkanSurface(VulkanAdapter* adapter, SurfaceDescriptor descriptor)
    : Surface(adapter, descriptor)
{
    createSurfaceKHR();

    // Gather sruface information.
    gatherSurfaceInfo();
}

VulkanSurface::~VulkanSurface()
{
    auto vulkanAdapter = static_cast<VulkanAdapter*>(m_adapter);
    const VulkanAPI& vkAPI = static_cast<VulkanDriver*>(vulkanAdapter->getDriver())->vkAPI;

    vkAPI.DestroySurfaceKHR(vulkanAdapter->getInstance(), m_surface, nullptr);
}

VkSurfaceKHR VulkanSurface::getSurface() const
{
    return m_surface;
}

const VulkanSurfaceInfo& VulkanSurface::getSurfaceInfo() const
{
    return m_surfaceInfo;
}

void VulkanSurface::gatherSurfaceInfo()
{
    auto vulkanAdapter = static_cast<VulkanAdapter*>(m_adapter);
    const VulkanAPI& vkAPI = static_cast<VulkanDriver*>(vulkanAdapter->getDriver())->vkAPI;

    VkPhysicalDevice physicalDevice = vulkanAdapter->getPhysicalDevice();
    vkAPI.GetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_surface, &m_surfaceInfo.capabilities);

    uint32_t surfaceFormatCount;
    vkAPI.GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface, &surfaceFormatCount, nullptr);
    if (surfaceFormatCount != 0)
    {
        m_surfaceInfo.formats.resize(surfaceFormatCount);
        vkAPI.GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface, &surfaceFormatCount, m_surfaceInfo.formats.data());
    }

    uint32_t presentModeCount;
    vkAPI.GetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_surface, &presentModeCount, nullptr);
    if (presentModeCount != 0)
    {
        m_surfaceInfo.presentModes.resize(presentModeCount);
        vkAPI.GetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_surface, &presentModeCount, m_surfaceInfo.presentModes.data());
    }
}

}; // namespace vkt
