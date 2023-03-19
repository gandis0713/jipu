#include "vulkan_platform.h"

namespace vkt
{

VulkanPlatform::VulkanPlatform(PlatformCreateHandles handles, PlatformCreateInfo info) noexcept
    : Platform(info), m_instance(handles.instance), m_physicalDevice(handles.physicalDevice)
{
}

} // namespace vkt
