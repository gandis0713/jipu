#include "platform_macos.h"
#include "utils/log.h"
#include "vk/vulkan_api.h"

#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>

namespace vkt
{

PlatformMacOS::PlatformMacOS(PlatformCreateInfo info) : Platform(info) {}
PlatformMacOS::~PlatformMacOS() {}

#if defined(VK_USE_PLATFORM_MACOS_MVK)
void* PlatformMacOS::createVkSurfaceKHR(void* nativeWindow, void* instance)
{

    @autoreleasepool
    {
        NSView* nsView = (__bridge NSView*)nativeWindow;
        if (nsView == nil)
        {
            LOG_ERROR("[{}] Failed to get NSView.", __func__);
            return nullptr;
        }
        NSBundle* bundle = [NSBundle bundleWithPath:@"/System/Library/Frameworks/QuartzCore.framework"];
        CAMetalLayer* layer = [[bundle classNamed:@"CAMetalLayer"] layer];

        [nsView setLayer:layer];

        VkMacOSSurfaceCreateInfoMVK createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
        createInfo.pView = (__bridge void*)nsView;

        VkSurfaceKHR surface{};
        VkResult result = vkCreateMacOSSurfaceMVK((VkInstance)instance, &createInfo, nullptr, &surface);

        if (result != VK_SUCCESS)
        {
            LOG_ERROR("Failed to create VkSurfaceKHR. {}", result);
        }

        return surface;
    }
}

#elif defined(VK_USE_PLATFORM_METAL_EXT)
void* PlatformMacOS::createVkSurfaceKHR(void* nativeWindow, void* instance)
{
    @autoreleasepool
    {
        NSView* nsView = (__bridge NSView*)nativeWindow;
        if (nsView == nil)
        {
            LOG_ERROR("[{}] Failed to get NSView.", __func__);
            return nullptr;
        }

        NSBundle* bundle = [NSBundle bundleWithPath:@"/System/Library/Frameworks/QuartzCore.framework"];
        CAMetalLayer* layer = [[bundle classNamed:@"CAMetalLayer"] layer];
        [nsView setLayer:layer];
        [nsView setWantsLayer:YES];

        PFN_vkCreateMetalSurfaceEXT vkCreateMetalSurfaceEXT;
        vkCreateMetalSurfaceEXT = (PFN_vkCreateMetalSurfaceEXT)vkGetInstanceProcAddr(static_cast<VkInstance>(instance), "vkCreateMetalSurfaceEXT");
        if (!vkCreateMetalSurfaceEXT)
        {
            LOG_ERROR("Cocoa: Vulkan instance missing VK_EXT_metal_surface "
                      "extension");
            return nullptr;
        }

        VkMetalSurfaceCreateInfoEXT surfaceCreateInfo{};
        surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_METAL_SURFACE_CREATE_INFO_EXT;
        surfaceCreateInfo.pLayer = layer;
        VkSurfaceKHR surface{};
        VkResult result = vkCreateMetalSurfaceEXT(static_cast<VkInstance>(instance), &surfaceCreateInfo, nullptr, &surface);

        if (result != VK_SUCCESS)
        {
            LOG_ERROR("failed to create window surface. {}", result);
        }

        return surface;
    }
}
#endif

} // namespace vkt
