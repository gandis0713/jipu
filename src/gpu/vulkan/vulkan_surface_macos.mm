#include "utils/log.h"
#include "vulkan_adapter.h"
#include "vulkan_api.h"
#include "vulkan_driver.h"
#include "vulkan_surface.h"

#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>
#include <fmt/format.h>

namespace vkt
{

#if defined(VK_USE_PLATFORM_METAL_EXT)
VkSurfaceKHR VulkanSurface::createSurfaceKHR()
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

        VulkanAdapter* adapter = static_cast<VulkanAdapter*>(m_adapter);
        const VulkanAPI& vkAPI = static_cast<VulkanDriver*>(m_adapter->getDriver())->vkAPI;
        if (vkAPI.CreateMetalSurfaceEXT == nullptr)
        {
            throw std::runtime_error("vkCreateMetalSurfaceEXT is nullptr.");
        }

        VkMetalSurfaceCreateInfoEXT surfaceCreateInfo{};
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
        surfaceCreateInfo.pLayer = layer;
        VkSurfaceKHR surface{};
        VkResult result = vkAPI.CreateMetalSurfaceEXT(adapter->getInstance(), &surfaceCreateInfo, nullptr, &surface);

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error(fmt::format("Failed to create VkSurfaceKHR.: {}", result));
        }

        return surface;
    }
}
#elif defined(VK_USE_PLATFORM_MACOS_MVK)
VkSurfaceKHR VulkanSurface::createSurfaceKHR()
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

        VulkanAdapter* adapter = static_cast<VulkanAdapter*>(m_adapter);
        const VulkanAPI& vkAPI = static_cast<VulkanDriver*>(m_adapter->getDriver())->vkAPI;
        if (vkAPI.CreateMacOSSurfaceMVK == nullptr)
        {
            throw std::runtime_error("vkCreateMacOSSurfaceMVK is nullptr.");
        }

        VkSurfaceKHR surface{};
        VkResult result = vkAPI.CreateMacOSSurfaceMVK(adapter->getInstance(), &createInfo, nullptr, &surface);

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error(fmt::format("Failed to create VkSurfaceKHR.: {}", result));
        }

        return surface;
    }
}

#endif

} // namespace vkt
