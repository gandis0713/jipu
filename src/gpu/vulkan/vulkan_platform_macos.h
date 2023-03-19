#pragma once

#include "vulkan_platform.h"

namespace vkt
{

class VulkanPlatformMacOS : public VulkanPlatform
{
public:
    VulkanPlatformMacOS(VulkanAdapter* adapter, PlatformCreateInfo info) noexcept;
    ~VulkanPlatformMacOS() override = default;

    virtual VkSurfaceKHR createSurfaceKHR(SurfaceCreateInfo info) override;
};

} // namespace vkt
