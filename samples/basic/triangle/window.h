#pragma once

#include <stdint.h>
#include <string>

namespace vkt
{

class Window
{
public:
    Window(int w, int h, std::string title);
    ~Window();

    int shouldClose();
    void* createSurface(void* instance);
    void getFrameBufferSize(int* w, int* h);

    void* getNativeWindow();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    void open();
    void close();

private:
    void* m_window;
};

} // namespace vkt
