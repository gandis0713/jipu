#pragma once

#include "vulkan_api.h"
#include "vulkan_platform.h"

namespace vkt
{
class VulkanPlatformWindows : public VulkanPlatform
{
public:
    VulkanPlatformWindows(VulkanAdapter* adapter, PlatformCreateInfo info) noexcept;
    ~VulkanPlatformWindows() override = default;

    VkSurfaceKHR createSurfaceKHR(SurfaceCreateInfo info) override;
};

} // namespace vkt
