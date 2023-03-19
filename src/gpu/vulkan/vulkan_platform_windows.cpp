#include "vulkan_platform_windows.h"

namespace vkt
{

VulkanPlatformWindows::VulkanPlatformWindows(PlatformCreateHandles handles, PlatformCreateInfo info) noexcept : VulkanPlatform(handles, info) {}

std::unique_ptr<Surface> VulkanPlatformWindows::createSurface(SurfaceCreateInfo info) { return nullptr; }

} // namespace vkt
