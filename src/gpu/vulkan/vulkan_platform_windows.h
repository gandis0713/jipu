#pragma once

#include "vulkan_platform.h"

namespace vkt
{
class VulkanPlatformWindows : public VulkanPlatform
{
public:
    VulkanPlatformWindows(VulkanAdapter* adapter, PlatformCreateInfo info) noexcept;
    ~VulkanPlatformWindows() override = default;

    std::unique_ptr<Surface> createSurface(SurfaceCreateInfo info) override;
};

} // namespace vkt
