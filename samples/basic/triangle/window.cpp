#include "window.h"
// clang-format off
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
// clang-format on
#include <spdlog/spdlog.h>

namespace vkt
{

Window::Window(int w, int h, std::string title) : m_nativeWindow(nullptr)
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    m_nativeWindow = static_cast<void*>(glfwCreateWindow(w, h, title.c_str(), nullptr, nullptr));
}

Window::~Window() {}

int Window::shouldClose()
{
    int ret = glfwWindowShouldClose(static_cast<GLFWwindow*>(m_nativeWindow));
    if(!ret)
    {
        glfwPollEvents();
    }

    return ret;
}

void* Window::createSurface(void* instance)
{

    VkSurfaceKHR surface{ nullptr };
    if (glfwCreateWindowSurface(static_cast<VkInstance>(instance), static_cast<GLFWwindow*>(m_nativeWindow), nullptr, &surface) != VK_SUCCESS)
    {
        spdlog::error("failed to create window surface!");
    }

    return surface;
}

void Window::getFrameBufferSize(int* w, int* h) { glfwGetFramebufferSize(static_cast<GLFWwindow*>(m_nativeWindow), w, h); }

} // namespace vkt
