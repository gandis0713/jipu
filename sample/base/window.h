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

    void setWidth(uint32_t width);
    void setHeight(uint32_t height);

    uint32_t getWidth() const;
    uint32_t getHeight() const;

    virtual void draw() = 0;
    virtual void init() = 0;

    bool isInitialized() const;

    int exec();
    void* getWindowHandle();

protected:
    void* m_handle = nullptr;
    bool m_initialized = false;

    uint32_t m_width = 0;
    uint32_t m_height = 0;
};
} // namespace vkt
