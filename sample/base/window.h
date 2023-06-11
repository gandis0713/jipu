#pragma once

#include <functional>
#include <stdint.h>
#include <string>

namespace vkt
{

struct WindowDescriptor
{
    uint32_t width = 0;
    uint32_t height = 0;
    std::string title = "";
    void* handle = nullptr;
};

class Window
{
public:
    Window(const WindowDescriptor& descriptor);
    virtual ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    virtual void draw() = 0;
    virtual void init() = 0;

    bool isInitialized() const;

    int exec();
    void* getWindowHandle();

protected:
    void* m_handle = nullptr;
    bool m_initialized = false;
};
} // namespace vkt
