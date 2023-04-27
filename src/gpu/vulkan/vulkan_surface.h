#pragma once

#include "gpu/surface.h"
#include "vulkan_api.h"
#include <vector>

namespace vkt
{

struct VulkanSurfaceInfo
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
    std::vector<bool> supportedQueueFamilies;
};

class VulkanAdapter;
class VulkanSurface : public Surface
{
public:
    VulkanSurface() = delete;
    VulkanSurface(VulkanAdapter* adapter, SurfaceDescriptor descriptor);
    ~VulkanSurface() override;

    VkSurfaceKHR getSurface() const;

    const VkSurfaceCapabilitiesKHR& getSurfaceCapabilities() const;
    const std::vector<VkSurfaceFormatKHR>& getSurfaceFormats() const;
    const std::vector<VkPresentModeKHR>& getPresentModes() const;

private:
    void createSurfaceKHR();

    void gatherSurfaceInfo();

private:
    // surface info.
    VkSurfaceCapabilitiesKHR m_surfaceCapabilities;
    std::vector<VkSurfaceFormatKHR> m_surfaceFormats;
    std::vector<VkPresentModeKHR> m_presentModes;

private:
    VkSurfaceKHR m_surface;
    VkPhysicalDevice m_physicalDevice;

    VulkanSurfaceInfo m_surfaceInfo{};
};

}; // namespace vkt
