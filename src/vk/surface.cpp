
#include "surface.h"

#include "utils/log.h"

namespace vkt
{

Surface::Surface(const SurfaceVulkanHandles handles, const SurfaceCreateInfo info) noexcept
    : m_physicalDevice(handles.physicalDevice), m_surface(handles.surface)
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

Surface::~Surface() noexcept {}

VkSurfaceKHR Surface::getSurface() const { return m_surface; }

const VkSurfaceCapabilitiesKHR& Surface::getSurfaceCapabilities() const { return m_surfaceCapabilities; }
const std::vector<VkSurfaceFormatKHR>& Surface::getSurfaceFormats() const { return m_surfaceFormats; }
const std::vector<VkPresentModeKHR>& Surface::getPresentModes() const { return m_presentModes; }

} // namespace vkt
