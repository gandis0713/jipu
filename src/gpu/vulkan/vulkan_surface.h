#pragma once

#include "gpu/surface.h"
#include "utils/cast.h"
#include "vulkan_api.h"

#include <vector>

namespace vkt
{

struct VulkanSurfaceInfo
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
    std::vector<bool> supportedQueueFamilies; // TODO: remove or not.
};

class VulkanAdapter;
class VulkanSurface : public Surface
{
public:
    VulkanSurface() = delete;
    VulkanSurface(VulkanAdapter* adapter, SurfaceDescriptor descriptor);
    ~VulkanSurface() override;

    VkSurfaceKHR getSurfaceKHR() const;
    const VulkanSurfaceInfo& getSurfaceInfo() const;

private:
    void createSurfaceKHR();
    void gatherSurfaceInfo();

private:
    VkSurfaceKHR m_surface{ VK_NULL_HANDLE };

    VulkanSurfaceInfo m_surfaceInfo{};
};

VULKAN_DOWNCAST(Surface);

}; // namespace vkt
