#pragma once

#include "vulkan_api.h"
#include "vulkan_platform.h"

namespace vkt
{
class VulkanPlatformWindows : public VulkanPlatform
{
public:
    VulkanPlatformWindows(VulkanAdapter* adapter, PlatformCreateInfo descriptor) noexcept;
    ~VulkanPlatformWindows() override = default;

    VkSurfaceKHR createSurfaceKHR(SurfaceCreateInfo descriptor) override;
};

} // namespace vkt
