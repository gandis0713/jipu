#pragma once

#include "gpu/surface.h"
#include "vulkan_api.h"
#include <vector>

namespace vkt
{

struct SurfaceCreateHandles
{
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice;
};

class VulkanSurface: public Surface
{
public:
    VulkanSurface(const SurfaceCreateHandles handles, const SurfaceCreateInfo info) noexcept;
    ~VulkanSurface() override = default;

    VkSurfaceKHR getSurface() const;

    const VkSurfaceCapabilitiesKHR& getSurfaceCapabilities() const;
    const std::vector<VkSurfaceFormatKHR>& getSurfaceFormats() const;
    const std::vector<VkPresentModeKHR>& getPresentModes() const;

private:
    // surface info.
    VkSurfaceCapabilitiesKHR m_surfaceCapabilities;
    std::vector<VkSurfaceFormatKHR> m_surfaceFormats;
    std::vector<VkPresentModeKHR> m_presentModes;

private:
    VkSurfaceKHR m_surface;
    VkPhysicalDevice m_physicalDevice;
};

}; // namespace vkt
