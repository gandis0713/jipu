#include "vulkan_platform_macos.h"
#include "utils/log.h"
#include "vulkan_adapter.h"
#include "vulkan_api.h"
#include "vulkan_surface.h"

#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>
#include <fmt/format.h>

namespace vkt
{

VulkanPlatformMacOS::VulkanPlatformMacOS(VulkanAdapter* adapter, PlatformCreateInfo info) noexcept : VulkanPlatform(adapter, info) {}

#if defined(VK_USE_PLATFORM_MACOS_MVK)
VkSurfaceKHR VulkanPlatformMacOS::createSurfaceKHR(SurfaceCreateInfo info)
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

        VkSurfaceKHR surface{};
        VulkanAdapter* adapter = static_cast<VulkanAdapter*>(m_adapter);
        VkResult result = vkCreateMacOSSurfaceMVK(adapter->getInstance(), &createInfo, nullptr, &surface);

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error(fmt::format("Failed to create VkSurfaceKHR.: {}", result));
        }

        return surface;
    }
}

#elif defined(VK_USE_PLATFORM_METAL_EXT)
VkSurfaceKHR VulkanPlatformMacOS::createSurfaceKHR(SurfaceCreateInfo info)
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

        PFN_vkCreateMetalSurfaceEXT vkCreateMetalSurfaceEXT;

        VulkanAdapter* adapter = static_cast<VulkanAdapter*>(m_adapter);
        vkCreateMetalSurfaceEXT = (PFN_vkCreateMetalSurfaceEXT)vkGetInstanceProcAddr(adapter->getInstance(), "vkCreateMetalSurfaceEXT");
        if (!vkCreateMetalSurfaceEXT)
        {
            throw std::runtime_error("Cocoa: Vulkan instance missing VK_EXT_metal_surface extension");
        }

        VkMetalSurfaceCreateInfoEXT surfaceCreateInfo{};
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
        surfaceCreateInfo.pLayer = layer;
        VkSurfaceKHR surface{};
        VkResult result = vkCreateMetalSurfaceEXT(adapter->getInstance(), &surfaceCreateInfo, nullptr, &surface);

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error(fmt::format("Failed to create VkSurfaceKHR.: {}", result));
        }

        return surface;
    }
}
#endif

} // namespace vkt
