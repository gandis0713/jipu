#include "vulkan_surface.h"
#include "vulkan_driver.h"

namespace vkt
{

VulkanSurface::VulkanSurface(VulkanDriver* driver, SurfaceDescriptor descriptor)
    : Surface(driver, descriptor)
{
    createSurfaceKHR();
}

VulkanSurface::~VulkanSurface()
{
    auto vulkanDriver = downcast(m_driver);
    const VulkanAPI& vkAPI = vulkanDriver->vkAPI;

    vkAPI.DestroySurfaceKHR(vulkanDriver->getInstance(), m_surface, nullptr);
}

VkSurfaceKHR VulkanSurface::getSurfaceKHR() const
{
    return m_surface;
}

VulkanSurfaceInfo VulkanSurface::gatherSurfaceInfo(VkPhysicalDevice physicalDevice) const
{
    // TODO: optimizing surfaceinfo caching or other way.

    VulkanSurfaceInfo surfaceInfo{};

    const VulkanAPI& vkAPI = downcast(m_driver)->vkAPI;
    vkAPI.GetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_surface, &surfaceInfo.capabilities);

    // Surface formats.
    {
        uint32_t surfaceFormatCount;
        vkAPI.GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface, &surfaceFormatCount, nullptr);

        surfaceInfo.formats.resize(surfaceFormatCount);
        vkAPI.GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface, &surfaceFormatCount, surfaceInfo.formats.data());
    }

    // Surface present modes.
    {
        uint32_t presentModeCount;
        vkAPI.GetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_surface, &presentModeCount, nullptr);

        surfaceInfo.presentModes.resize(presentModeCount);
        vkAPI.GetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_surface, &presentModeCount, surfaceInfo.presentModes.data());
    }

    return surfaceInfo;
}

}; // namespace vkt
