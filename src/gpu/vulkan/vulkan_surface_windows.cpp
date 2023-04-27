#include "vulkan_adapter.h"
#include "vulkan_surface.h"

#include <fmt/format.h>
#include <stdexcept>

namespace vkt
{

VkSurfaceKHR VulkanSurface::createSurfaceKHR()
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
