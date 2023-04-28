#include "vulkan_adapter.h"
#include "vulkan_surface.h"

#include <fmt/format.h>
#include <stdexcept>

namespace vkt
{

void VulkanSurface::createSurfaceKHR()
{
    VkWin32SurfaceCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
    createInfo.hwnd = (HWND)m_windowHandle;
    createInfo.hinstance = GetModuleHandle(nullptr);

    VulkanAdapter* adapter = static_cast<VulkanAdapter*>(m_adapter);
    auto driver = static_cast<VulkanDriver*>(m_adapter->getDriver());
    VkResult result = driver->vkAPI.CreateWin32SurfaceKHR(adapter->getInstance(), &createInfo, nullptr, &m_surface);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error(fmt::format("Failed to create VkSurfaceKHR.: {}", result));
    }
}

} // namespace vkt
