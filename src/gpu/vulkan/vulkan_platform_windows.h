#pragma once

#include "vulkan_api.h"
#include "vulkan_platform.h"

namespace vkt
{
class VulkanPlatformWindows : public VulkanPlatform
{
public:
    VulkanPlatformWindows(VulkanAdapter* adapter, PlatformDescriptor descriptor) noexcept;
    ~VulkanPlatformWindows() override = default;

    VkSurfaceKHR createSurfaceKHR(SurfaceDescriptor descriptor) override;
};

} // namespace vkt
