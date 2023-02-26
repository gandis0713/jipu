#include "platform_macos.h"
#include "vk/precompile.h"

#include <Cocoa/Cocoa.h>
// #import <Metal/Metal.h>
// #import <QuartzCore/CAMetalLayer.h>

namespace vkt {

void *PlatformMacOS::createSurface(void *nativeWindow, void *instance) {
    NSView *nsview = (__bridge NSView *)nativeWindow;

    VkSurfaceKHR surface{};
    VkMacOSSurfaceCreateInfoMVK createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
    createInfo.pView = (__bridge void *)nsview;
    VkResult result = vkCreateMacOSSurfaceMVK((VkInstance)instance, &createInfo,
                                              nullptr, &surface);
    // static_assert(result == VK_SUCCESS);
    return surface;
}

} // namespace vkt
