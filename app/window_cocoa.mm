#include "window.h"
// clang-format off
#include "vkt/gpu/vulkan/vulkan_api.h"
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

Window::Window(int w, int h, std::string title)
    : m_window(nullptr)
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    m_window = static_cast<void*>(glfwCreateWindow(w, h, title.c_str(), nullptr, nullptr));
}

Window::~Window()
{
    glfwTerminate();
}

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

void* Window::getNativeWindow()
{
    return getNSView(static_cast<GLFWwindow*>(m_window));
}

void Window::getFrameBufferSize(int* w, int* h)
{
    glfwGetFramebufferSize(static_cast<GLFWwindow*>(m_window), w, h);
}
