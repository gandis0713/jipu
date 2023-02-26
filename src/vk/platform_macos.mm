#include "platform_macos.h"
#include "vk/precompile.h"
#include <spdlog/spdlog.h>

#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>

namespace vkt
{

void* PlatformMacOS::createVkSurfaceKHR(void* nativeWindow, void* instance)
{
    @autoreleasepool
    {
        NSView* nsView = (__bridge NSView*)nativeWindow;
        if (nsView == nil)
        {
            spdlog::error("[{}] Failed to get NSView.", __func__);
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
            spdlog::error("Cocoa: Vulkan instance missing VK_EXT_metal_surface "
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
            spdlog::error("failed to create window surface. {}", result);
        }

        return surface;
    }
}

} // namespace vkt
