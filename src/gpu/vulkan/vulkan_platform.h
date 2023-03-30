#pragma once

#include "gpu/platform.h"

#include "vulkan_api.h"

#include <memory>

namespace vkt
{

class VulkanAdapter;

class VulkanPlatform : public Platform
{
public:
    VulkanPlatform(VulkanAdapter* adapter, PlatformDescriptor descriptor) noexcept;
    ~VulkanPlatform() override = default;

    std::unique_ptr<Surface> createSurface(SurfaceDescriptor descriptor) override;

public:
    virtual VkSurfaceKHR createSurfaceKHR(SurfaceDescriptor descriptor) = 0;
};

} // namespace vkt
