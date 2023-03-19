#pragma once

#include "vulkan_platform.h"

namespace vkt
{

class VulkanPlatformMacOS : public VulkanPlatform
{
public:
    VulkanPlatformMacOS(VulkanAdapter* adapter, PlatformCreateInfo info) noexcept;
    ~VulkanPlatformMacOS() override = default;

    std::unique_ptr<Surface> createSurface(SurfaceCreateInfo info) override;
};

} // namespace vkt
