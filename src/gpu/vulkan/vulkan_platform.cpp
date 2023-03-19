#include "vulkan_platform.h"

#include "vulkan_adapter.h"

namespace vkt
{

VulkanPlatform::VulkanPlatform(VulkanAdapter* adapter, PlatformCreateInfo info) noexcept : Platform(adapter, info) {}

} // namespace vkt
