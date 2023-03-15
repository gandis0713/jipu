#pragma once

#include "context.h"
#include <vector>

namespace vkt
{

struct SurfaceCreateInfo
{
    VkPhysicalDevice physicalDevice;
    VkSurfaceKHR surface; // TODO: replace Platform.
};

class Surface
{
public:
    explicit Surface(const SurfaceCreateInfo info) noexcept;
    ~Surface() noexcept;

    void* getHandle() const;

    const VkSurfaceCapabilitiesKHR& getSurfaceCapabilities() const;
    const std::vector<VkSurfaceFormatKHR>& getSurfaceFormats() const;
    const std::vector<VkPresentModeKHR>& getPresentModes() const;

private:
    // surface info.
    VkSurfaceCapabilitiesKHR m_surfaceCapabilities;
    std::vector<VkSurfaceFormatKHR> m_surfaceFormats;
    std::vector<VkPresentModeKHR> m_presentModes;

private:
    VkSurfaceKHR m_handle;
    VkPhysicalDevice m_physicalDevice;
};

}; // namespace vkt
