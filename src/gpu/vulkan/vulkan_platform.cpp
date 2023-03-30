#include "vulkan_platform.h"

#include "vulkan_adapter.h"
#include "vulkan_surface.h"

namespace vkt
{

VulkanPlatform::VulkanPlatform(VulkanAdapter* adapter, PlatformDescriptor descriptor) noexcept
    : Platform(adapter, descriptor)
{
}

std::unique_ptr<Surface> VulkanPlatform::createSurface(SurfaceDescriptor descriptor)
{
    return std::make_unique<VulkanSurface>(this, descriptor);
}

} // namespace vkt
