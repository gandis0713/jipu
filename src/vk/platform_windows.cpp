#include "platform_windows.h"

namespace vkt
{

PlatformWindows::PlatformWindows(PlatformVulkanHandles handles, PlatformCreateInfo info) noexcept : Platform(handles, info) {}

PlatformWindows::~PlatformWindows() {}

std::unique_ptr<Surface> PlatformWindows::createSurface(SurfaceCreateInfo info) { return nullptr; }

} // namespace vkt
