#pragma once

#include <functional>
#include <stdint.h>
#include <string>

struct WindowDescriptor
{
    uint32_t width = 0;
    uint32_t height = 0;
    std::string title = "";
};

class Window
{
public:
    Window(const WindowDescriptor& descriptor, void* handle = nullptr);
    virtual ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    virtual void draw() = 0;

    int exec();
    void* getWindowHandle();

protected:
    void* m_handle = nullptr;
    std::function<void()> onDraw;
};
