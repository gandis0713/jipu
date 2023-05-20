#include "window.h"
// clang-format off
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
// clang-format on
#include "utils/log.h"

Window::Window(int w, int h, std::string title)
    : m_window(nullptr)
{
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

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
    return glfwGetWin32Window(static_cast<GLFWwindow*>(m_window));
}

void Window::getFrameBufferSize(int* w, int* h)
{
    glfwGetFramebufferSize(static_cast<GLFWwindow*>(m_window), w, h);
}
