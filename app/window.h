#pragma once

#include <stdint.h>
#include <string>

class Window
{
public:
    Window(int w, int h, std::string title);
    ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    void open();
    void close();

    int shouldClose();
    void* createSurface(void* instance);
    void getFrameBufferSize(int* w, int* h);

    void* getNativeWindow();

private:
    void* m_window;
};
