
#include "vulkan_surface.h"
#include "vulkan_adapter.h"
#include "vulkan_platform.h"

#include "utils/log.h"

namespace vkt
{

VulkanSurface::VulkanSurface(VulkanPlatform* vulkanPlatform, const SurfaceDescriptor descriptor)
    : Surface(vulkanPlatform, descriptor)
{
    SurfaceDescriptor surfaceDescriptor{};
    m_surface = vulkanPlatform->createSurfaceKHR(surfaceDescriptor);

    VulkanAdapter* adapter = static_cast<VulkanAdapter*>(vulkanPlatform->getAdapter());
    VkPhysicalDevice physicalDevice = adapter->getPhysicalDevice();

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_surface, &m_surfaceCapabilities);

    uint32_t surfaceFormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface, &surfaceFormatCount, nullptr);

    if (surfaceFormatCount != 0)
    {
        m_surfaceFormats.resize(surfaceFormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface, &surfaceFormatCount, m_surfaceFormats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_surface, &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        m_presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_surface, &presentModeCount, m_presentModes.data());
    }
}
VulkanSurface::~VulkanSurface()
{
    auto vulkanAdapter = static_cast<VulkanAdapter*>(m_platform->getAdapter());

    vkDestroySurfaceKHR(vulkanAdapter->getInstance(), m_surface, nullptr);
}

VkSurfaceKHR VulkanSurface::getSurface() const
{
    return m_surface;
}

const VkSurfaceCapabilitiesKHR& VulkanSurface::getSurfaceCapabilities() const
{
    return m_surfaceCapabilities;
}

const std::vector<VkSurfaceFormatKHR>& VulkanSurface::getSurfaceFormats() const
{
    return m_surfaceFormats;
}

const std::vector<VkPresentModeKHR>& VulkanSurface::getPresentModes() const
{
    return m_presentModes;
}

} // namespace vkt
