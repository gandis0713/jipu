#include "window.h"
// clang-format off
#include "vk/vulkan_api.h"
// glfw
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#include <GLFW/glfw3native.h>
// clang-format on

#include "utils/log.h"

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
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_window = static_cast<void*>(glfwCreateWindow(w, h, title.c_str(), nullptr, nullptr));
}

Window::~Window() {}

void Window::open()
{
    // TODO
}
void Window::close()
{
    // TODO
}

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

#if defined(VK_USE_PLATFORM_MACOS_MVK)
void* Window::createSurface(void* instance)
{
    @autoreleasepool
    {
        NSBundle* bundle = [NSBundle bundleWithPath:@"/System/Library/Frameworks/QuartzCore.framework"];
        CAMetalLayer* layer = [[bundle classNamed:@"CAMetalLayer"] layer];

        NSWindow* nsWindow = (__bridge NSWindow*)glfwGetCocoaWindow(static_cast<GLFWwindow*>(m_window));
        NSView* nsView = [nsWindow contentView];
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
void* Window::createSurface(void* instance)
{
    @autoreleasepool
    {
        NSBundle* bundle = [NSBundle bundleWithPath:@"/System/Library/Frameworks/QuartzCore.framework"];
        CAMetalLayer* layer = [[bundle classNamed:@"CAMetalLayer"] layer];

        NSView* nsView = getNSView(static_cast<GLFWwindow*>(m_window));
        if (nsView == nil)
        {
            LOG_ERROR("[{}] Failed to get NSView.", __func__);
            return nullptr;
        }
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