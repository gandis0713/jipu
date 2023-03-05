
#include "surface.h"

#include "utils/log.h"

namespace vkt
{

Surface::Surface(const SurfaceCreateInfo info) noexcept : m_physicalDevice(info.physicalDevice), m_handle(info.surface)
{
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice, m_handle, &m_surfaceCapabilities);

    uint32_t surfaceFormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, m_handle, &surfaceFormatCount, nullptr);

    if (surfaceFormatCount != 0)
    {
        m_surfaceFormats.resize(surfaceFormatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(m_physicalDevice, m_handle, &surfaceFormatCount, m_surfaceFormats.data());
    }

    uint32_t presentModeCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, m_handle, &presentModeCount, nullptr);

    if (presentModeCount != 0)
    {
        m_presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(m_physicalDevice, m_handle, &presentModeCount, m_presentModes.data());
    }
}

Surface::~Surface() noexcept {}

void* Surface::getHandle() const
{
    return m_handle;
}

const VkSurfaceCapabilitiesKHR& Surface::getSurfaceCapabilities() const { return m_surfaceCapabilities; }
const std::vector<VkSurfaceFormatKHR>& Surface::getSurfaceFormats() const { return m_surfaceFormats; }
const std::vector<VkPresentModeKHR>& Surface::getPresentModes() const { return m_presentModes; }

} // namespace vkt
