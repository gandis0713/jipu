#pragma once

#include "export.h"

#include "jipu/surface.h"
#include "utils/cast.h"
#include "vulkan_api.h"

#include <vector>

namespace jipu
{

struct VulkanSurfaceInfo
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
    std::vector<bool> supportedQueueFamilies; // TODO: remove or not.
};

class VulkanDriver;
class JIPU_EXPERIMENTAL_EXPORT VulkanSurface : public Surface
{
public:
    VulkanSurface() = delete;
    VulkanSurface(VulkanDriver* driver, SurfaceDescriptor descriptor);
    ~VulkanSurface() override;

    VkSurfaceKHR getSurfaceKHR() const;
    VulkanSurfaceInfo gatherSurfaceInfo(VkPhysicalDevice physicalDevice) const;

private:
    void createSurfaceKHR();

private:
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;

    VulkanSurfaceInfo m_surfaceInfo{};
};

DOWN_CAST(VulkanSurface, Surface);

}; // namespace jipu
