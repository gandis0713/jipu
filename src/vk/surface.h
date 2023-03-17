#pragma once

#include "vulkan_api.h"
#include <vector>

namespace vkt
{

struct SurfaceCreateInfo
{
};

struct SurfaceCreateHandles
{
    VkSurfaceKHR surface;
    VkPhysicalDevice physicalDevice;
};

class Surface
{
public:
    explicit Surface(const SurfaceCreateHandles handles, const SurfaceCreateInfo info) noexcept;
    ~Surface() noexcept;

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
