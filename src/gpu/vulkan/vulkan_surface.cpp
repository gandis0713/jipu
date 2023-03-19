
#include "vulkan_surface.h"

#include "utils/log.h"

namespace vkt
{

VulkanSurface::VulkanSurface(const SurfaceCreateHandles handles, const SurfaceCreateInfo info) noexcept
    : Surface(info), m_physicalDevice(handles.physicalDevice), m_surface(handles.surface)
{
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice, m_surface, &m_surfaceCapabilities);

    uint32_t surfaceFormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, m_surface, &surfaceFormatCount, nullptr);

    if (surfaceFormatCount != 0)
    {
        m_surfaceFormats.resize(surfaceFormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, m_surface, &surfaceFormatCount, m_surfaceFormats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, m_surface, &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        m_presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, m_surface, &presentModeCount, m_presentModes.data());
    }
}

VkSurfaceKHR VulkanSurface::getSurface() const { return m_surface; }

const VkSurfaceCapabilitiesKHR& VulkanSurface::getSurfaceCapabilities() const { return m_surfaceCapabilities; }
const std::vector<VkSurfaceFormatKHR>& VulkanSurface::getSurfaceFormats() const { return m_surfaceFormats; }
const std::vector<VkPresentModeKHR>& VulkanSurface::getPresentModes() const { return m_presentModes; }

} // namespace vkt
