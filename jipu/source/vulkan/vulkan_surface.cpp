#include "vulkan_surface.h"
#include "vulkan_driver.h"

#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace jipu
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

    vkAPI.DestroySurfaceKHR(vulkanDriver->getVkInstance(), m_surface, nullptr);
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
    VkResult result = vkAPI.GetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, m_surface, &surfaceInfo.capabilities);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error(fmt::format("Failure GetPhysicalDeviceSurfaceCapabilitiesKHR Error: {}", static_cast<int32_t>(result)));
    }

    // Surface formats.
    {
        uint32_t surfaceFormatCount;
        result = vkAPI.GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface, &surfaceFormatCount, nullptr);
        if (result != VK_SUCCESS && result != VK_INCOMPLETE)
        {
            throw std::runtime_error(fmt::format("Failure GetPhysicalDeviceSurfaceFormatsKHR to get count. Error: {}", static_cast<int32_t>(result)));
        }

        surfaceInfo.formats.resize(surfaceFormatCount);
        result = vkAPI.GetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, m_surface, &surfaceFormatCount, surfaceInfo.formats.data());
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error(fmt::format("Failure GetPhysicalDeviceSurfaceFormatsKHR. Error: {}", static_cast<int32_t>(result)));
        }

        // for (const auto& format : surfaceInfo.formats)
        // {
        //     spdlog::trace("Surface format, colorspace: {}, {}",
        //                   static_cast<uint32_t>(format.format),
        //                   static_cast<uint32_t>(format.colorSpace));
        // }
    }

    // Surface present modes.
    {
        uint32_t presentModeCount;
        result = vkAPI.GetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_surface, &presentModeCount, nullptr);
        if (result != VK_SUCCESS && result != VK_INCOMPLETE)
        {
            throw std::runtime_error(fmt::format("Failure GetPhysicalDeviceSurfacePresentModesKHR to get count. Error: {}", static_cast<int32_t>(result)));
        }

        surfaceInfo.presentModes.resize(presentModeCount);
        result = vkAPI.GetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, m_surface, &presentModeCount, surfaceInfo.presentModes.data());
        if (result != VK_SUCCESS)
        {
            throw std::runtime_error(fmt::format("Failure GetPhysicalDeviceSurfacePresentModesKHR. Error: {}", static_cast<int32_t>(result)));
        }
    }

    return surfaceInfo;
}

}; // namespace jipu
