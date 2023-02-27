#include "window.h"
// clang-format off
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
// clang-format on
#include <spdlog/spdlog.h>

namespace vkt
{

Window::Window(int w, int h, std::string title) : m_window(nullptr)
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
}

Window::~Window() {}

void Window::open()
{
    if (m_window != nullptr)
    {
        spdlog::warn("Window already was opened.");
        return;
    }

    m_window = static_cast<void*>(glfwCreateWindow(w, h, title.c_str(), nullptr, nullptr));
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
    // return glfwGetCocoaWindow(m_window);
    return nullptr;
}

void* Window::createSurface(void* instance)
{

    VkSurfaceKHR surface{ nullptr };
    if (glfwCreateWindowSurface(static_cast<VkInstance>(instance), static_cast<GLFWwindow*>(m_window), nullptr, &surface) != VK_SUCCESS)
    {
        spdlog::error("failed to create window surface!");
    }

    return surface;
}

void Window::getFrameBufferSize(int* w, int* h) { glfwGetFramebufferSize(static_cast<GLFWwindow*>(m_window), w, h); }

} // namespace vkt
