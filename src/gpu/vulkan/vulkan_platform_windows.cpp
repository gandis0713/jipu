#include "vulkan_platform_windows.h"

#include "vulkan_adapter.h"

#include <stdexcept>
#include <fmt/format.h>

namespace vkt
{

VulkanPlatformWindows::VulkanPlatformWindows(VulkanAdapter* adapter, PlatformCreateInfo info) noexcept : VulkanPlatform(adapter, info) {}

VkSurfaceKHR VulkanPlatformWindows::createSurfaceKHR(SurfaceCreateInfo info)
{
    VkWin32SurfaceCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hwnd = (HWND)m_windowHandle;
    createInfo.hinstance = GetModuleHandle(nullptr);

    VkSurfaceKHR surface{};
    VulkanAdapter* adapter = static_cast<VulkanAdapter*>(m_adapter);
    VkResult result = vkCreateWin32SurfaceKHR(adapter->getInstance(), &createInfo, nullptr, &surface);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error(fmt::format("Failed to create VkSurfaceKHR.: {}", result));
    }

    return surface;
}

} // namespace vkt
