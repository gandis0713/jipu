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
    auto vulkanAdapter = downcast(m_adapter);
    const VulkanAPI& vkAPI = downcast(vulkanAdapter->getDriver())->vkAPI;

    vkAPI.DestroySurfaceKHR(vulkanAdapter->getInstance(), m_surface, nullptr);
}

VkSurfaceKHR VulkanSurface::getSurfaceKHR() const
{
    return m_surface;
}

const VulkanSurfaceInfo& VulkanSurface::getSurfaceInfo() const
{
    return m_surfaceInfo;
}

void VulkanSurface::gatherSurfaceInfo()
{
    auto vulkanAdapter = downcast(m_adapter);
    const VulkanAPI& vkAPI = downcast(vulkanAdapter->getDriver())->vkAPI;

    VkPhysicalDevice physicalDevice = vulkanAdapter->getPhysicalDevice();
    vkAPI.GetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_surface, &m_surfaceInfo.capabilities);

    // Surface formats.
    {
        uint32_t surfaceFormatCount;
        vkAPI.GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface, &surfaceFormatCount, nullptr);

        m_surfaceInfo.formats.resize(surfaceFormatCount);
        vkAPI.GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface, &surfaceFormatCount, m_surfaceInfo.formats.data());
    }

    // Surface present modes.
    {
        uint32_t presentModeCount;
        vkAPI.GetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_surface, &presentModeCount, nullptr);

        m_surfaceInfo.presentModes.resize(presentModeCount);
        vkAPI.GetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_surface, &presentModeCount, m_surfaceInfo.presentModes.data());
    }
}

}; // namespace vkt
