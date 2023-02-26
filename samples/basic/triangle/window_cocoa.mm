#include "window.h"
// clang-format off
#include "vk/precompile.h"
// glfw
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>
// clang-format on

#include <spdlog/spdlog.h>

#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>

static NSView* getNSView(GLFWwindow* window)
{
    NSWindow* nsWindow = (__bridge NSWindow*)glfwGetCocoaWindow(static_cast<GLFWwindow*>(window));
    return [nsWindow contentView];
}

namespace vkt
{

Window::Window(int w, int h, std::string title) : m_window(nullptr)
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_window = static_cast<void*>(glfwCreateWindow(w, h, title.c_str(), nullptr, nullptr));
}

Window::~Window() {}

int Window::shouldClose()
{
    int ret = glfwWindowShouldClose(static_cast<GLFWwindow*>(m_window));
    if (!ret)
    {
        glfwPollEvents();
    }

    return ret;
}

void* Window::getNativeWindow() { return getNSView(static_cast<GLFWwindow*>(m_window)); }

void Window::getFrameBufferSize(int* w, int* h) { glfwGetFramebufferSize(static_cast<GLFWwindow*>(m_window), w, h); }

void* Window::createSurface(void* instance)
{
    @autoreleasepool
    {
        NSBundle* bundle = [NSBundle bundleWithPath:@"/System/Library/Frameworks/QuartzCore.framework"];
        CAMetalLayer* layer = [[bundle classNamed:@"CAMetalLayer"] layer];

        NSView* nsView = getNSView(static_cast<GLFWwindow*>(m_window));
        if (nsView == nil)
        {
            spdlog::error("[{}] Failed to get NSView.", __func__);
            return nullptr;
        }
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

// void *Window::createSurface(void *instance) {

//     @autoreleasepool {
//         NSWindow *nsWindow = (__bridge NSWindow *)glfwGetCocoaWindow(
//             static_cast<GLFWwindow *>(m_window));
//         NSView *nsView = [nsWindow contentView];
//         [nsView setLayer:[CAMetalLayer layer]];

//         VkMacOSSurfaceCreateInfoMVK createInfo{};
//         createInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
//         createInfo.pLayer = (__bridge void *)[CAMetalLayer layer];

//         VkSurfaceKHR surface{};
//         VkResult result = vkCreateMacOSSurfaceMVK(
//             (VkInstance)instance, &createInfo, nullptr, &surface);

//         if (result != VK_SUCCESS) {
//             spdlog::error("Failed to create VkSurfaceKHR. {}", result);
//         }

//         return surface;
//     }
// }

} // namespace vkt
