#include "vulkan_platform_macos.h"
#include "vulkan_surface.h"
#include "utils/log.h"
#include "vulkan_api.h"

#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>
#include <fmt/format.h>

namespace vkt
{

VulkanPlatformMacOS::VulkanPlatformMacOS(PlatformCreateHandles handles, PlatformCreateInfo info) noexcept : VulkanPlatform(handles, info) {}

#if defined(VK_USE_PLATFORM_MACOS_MVK)
std::unique_ptr<Surface> VulkanPlatformMacOS::createSurface(SurfaceCreateInfo info)
{
    @autoreleasepool
    {
        NSView* nsView = (__bridge NSView*)nativeWindow;
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
        VkResult result = vkCreateMacOSSurfaceMVK(m_instance, &createInfo, nullptr, &surface);

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error(fmt::format("Failed to create VkSurfaceKHR.: {}", result));
        }

        SurfaceCreateInfo info{ m_physicalDevice, vulkanSurface };

        return std::make_unique<Surface>(info);
    }
}

#elif defined(VK_USE_PLATFORM_METAL_EXT)
std::unique_ptr<Surface> VulkanPlatformMacOS::createSurface(SurfaceCreateInfo info)
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

        vkCreateMetalSurfaceEXT = (PFN_vkCreateMetalSurfaceEXT)vkGetInstanceProcAddr(m_instance, "vkCreateMetalSurfaceEXT");
        if (!vkCreateMetalSurfaceEXT)
        {
            throw std::runtime_error("Cocoa: Vulkan instance missing VK_EXT_metal_surface extension");
        }

        VkMetalSurfaceCreateInfoEXT surfaceCreateInfo{};
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
        surfaceCreateInfo.pLayer = layer;
        VkSurfaceKHR vulkanSurface{};
        VkResult result = vkCreateMetalSurfaceEXT(m_instance, &surfaceCreateInfo, nullptr, &vulkanSurface);

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error(fmt::format("Failed to create VkSurfaceKHR.: {}", result));
        }

        SurfaceCreateInfo info{};
        SurfaceCreateHandles handles{ vulkanSurface, m_physicalDevice };

        return std::make_unique<Surface>(handles, info);
    }
}
#endif

} // namespace vkt
