#include "vulkan_platform_windows.h"

#include "vulkan_adapter.h"

namespace vkt
{

VulkanPlatformWindows::VulkanPlatformWindows(VulkanAdapter* adapter, PlatformCreateInfo info) noexcept : VulkanPlatform(adapter, info) {}

std::unique_ptr<Surface> VulkanPlatformWindows::createSurface(SurfaceCreateInfo info) { return nullptr; }

} // namespace vkt
