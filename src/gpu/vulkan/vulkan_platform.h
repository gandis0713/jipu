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
    VulkanPlatform(VulkanAdapter* adapter, PlatformCreateInfo info) noexcept;
    ~VulkanPlatform() override = default;

    std::unique_ptr<Surface> createSurface(SurfaceCreateInfo info) override;

public:
    virtual VkSurfaceKHR createSurfaceKHR(SurfaceCreateInfo info) = 0;
};

} // namespace vkt
