#include "platform_macos.h"
#include "utils/log.h"
#include "vk/vulkan_api.h"

#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>
#include <fmt/format.h>

namespace vkt
{

PlatformMacOS::PlatformMacOS(PlatformCreateInfo info) : Platform(info) {}
PlatformMacOS::~PlatformMacOS() {}

#if defined(VK_USE_PLATFORM_MACOS_MVK)
std::shared_ptr<Surface> PlatformMacOS::createSurface(void* nativeWindow)
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

        std::shared_ptr<Driver> driver = m_adapter->getDriver();
        VkInstance instance = driver->getInstance();

        VkSurfaceKHR surface{};
        VkResult result = vkCreateMacOSSurfaceMVK((VkInstance)instance, &createInfo, nullptr, &surface);

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error(fmt::format("Failed to create VkSurfaceKHR.: {}", result));
        }

        SurfaceCreateInfo info{ nullptr, vulkanSurface };

        return std::make_shared<Surface>(info);
    }
}

#elif defined(VK_USE_PLATFORM_METAL_EXT)
std::shared_ptr<Surface> PlatformMacOS::createSurface(void* nativeWindow)
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
        [nsView setWantsLayer:YES];

        PFN_vkCreateMetalSurfaceEXT vkCreateMetalSurfaceEXT;

        std::shared_ptr<Driver> driver = m_adapter.getDriver();
        VkInstance instance = driver->getInstance();

        vkCreateMetalSurfaceEXT = (PFN_vkCreateMetalSurfaceEXT)vkGetInstanceProcAddr(static_cast<VkInstance>(instance), "vkCreateMetalSurfaceEXT");
        if (!vkCreateMetalSurfaceEXT)
        {
            throw std::runtime_error("Cocoa: Vulkan instance missing VK_EXT_metal_surface extension");
        }

        VkMetalSurfaceCreateInfoEXT surfaceCreateInfo{};
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
        surfaceCreateInfo.pLayer = layer;
        VkSurfaceKHR vulkanSurface{};
        VkResult result = vkCreateMetalSurfaceEXT(static_cast<VkInstance>(instance), &surfaceCreateInfo, nullptr, &vulkanSurface);

        if (result != VK_SUCCESS)
        {
            throw std::runtime_error(fmt::format("Failed to create VkSurfaceKHR.: {}", result));
        }

        SurfaceCreateInfo info{ m_adapter.getPhysicalDevice(), vulkanSurface };

        return std::make_shared<Surface>(info);
    }
}
#endif

} // namespace vkt
