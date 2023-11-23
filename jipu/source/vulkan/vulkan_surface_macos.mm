#include "vulkan_api.h"
#include "vulkan_driver.h"
#include "vulkan_surface.h"

#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>
#include <fmt/format.h>
#include <spdlog/spdlog.h>

namespace jipu
{

#if defined(VK_USE_PLATFORM_METAL_EXT)
void VulkanSurface::createSurfaceKHR()
{
    @autoreleasepool
    {
        NSView* nsView = (__bridge NSView*)m_windowHandle;
        if (nsView == nil)
        {
            throw std::runtime_error(fmt::format("[{}] Failed to get NSView.", __func__));
        }

        NSBundle* bundle = [NSBundle bundleWithPath:@"/System/Library/Frameworks/QuartzCore.framework"];
        CAMetalLayer* layer = [[bundle classNamed:@"CAMetalLayer"] layer];
        [nsView setLayer:layer];
        [nsView setWantsLayer:YES];

        VulkanDriver* driver = downcast(m_driver);
        const VulkanAPI& vkAPI = driver->vkAPI;
        if (vkAPI.CreateMetalSurfaceEXT == nullptr)
        {
            throw std::runtime_error("vkCreateMetalSurfaceEXT is nullptr.");
        }

        VkMetalSurfaceCreateInfoEXT surfaceCreateInfo{};
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
        surfaceCreateInfo.pLayer = layer;

        VkResult result = vkAPI.CreateMetalSurfaceEXT(driver->getVkInstance(), &surfaceCreateInfo, nullptr, &m_surface);

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error(fmt::format("Failed to create VkSurfaceKHR.: {}", static_cast<int32_t>(result)));
        }
    }
}
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
void VulkanSurface::createSurfaceKHR()
{
    @autoreleasepool
    {
        NSView* nsView = (__bridge NSView*)m_windowHandle;
        if (nsView == nil)
        {
            throw std::runtime_error(fmt::format("[{}] Failed to get NSView.", __func__));
        }
        NSBundle* bundle = [NSBundle bundleWithPath:@"/System/Library/Frameworks/QuartzCore.framework"];
        CAMetalLayer* layer = [[bundle classNamed:@"CAMetalLayer"] layer];

        [nsView setLayer:layer];

        VkMacOSSurfaceCreateInfoMVK createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
        createInfo.pView = (__bridge void*)nsView;

        VulkanDriver* driver = downcast(m_driver);
        const VulkanAPI& vkAPI = driver->vkAPI;
        if (vkAPI.CreateMacOSSurfaceMVK == nullptr)
        {
            throw std::runtime_error("vkCreateMacOSSurfaceMVK is nullptr.");
        }

        VkResult result = vkAPI.CreateMacOSSurfaceMVK(driver->getVkInstance(), &createInfo, nullptr, &m_surface);

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error(fmt::format("Failed to create VkSurfaceKHR.: {}", static_cast<int32_t>(result)));
        }
    }
}

#endif

} // namespace jipu
