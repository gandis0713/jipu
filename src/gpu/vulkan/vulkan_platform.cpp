#include "vulkan_platform.h"

#include "vulkan_adapter.h"
#include "vulkan_surface.h"

namespace vkt
{

VulkanPlatform::VulkanPlatform(VulkanAdapter* adapter, PlatformCreateInfo info) noexcept
    : Platform(adapter, info)
{
}

std::unique_ptr<Surface> VulkanPlatform::createSurface(SurfaceCreateInfo info)
{
    return std::make_unique<VulkanSurface>(this, info);
}

} // namespace vkt
