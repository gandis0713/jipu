#pragma once

#include "gpu/surface.h"
#include "vulkan_api.h"
#include <vector>

namespace vkt
{

class VulkanPlatform;

class VulkanSurface : public Surface
{
public:
    VulkanSurface() = delete;
    VulkanSurface(VulkanPlatform* platform, SurfaceCreateInfo info);
    ~VulkanSurface() override;

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
