#pragma once

#include "vulkan_platform.h"

namespace vkt
{

class VulkanPlatformMacOS : public VulkanPlatform
{
public:
    VulkanPlatformMacOS(VulkanAdapter* adapter, PlatformDescriptor descriptor) noexcept;
    ~VulkanPlatformMacOS() override = default;

    virtual VkSurfaceKHR createSurfaceKHR(SurfaceDescriptor descriptor) override;
};

} // namespace vkt
