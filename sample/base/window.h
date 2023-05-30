#pragma once

#include <stdint.h>
#include <string>

class Window
{
public:
    Window(int w, int h, std::string title);
    virtual ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;

    int exec();

    virtual void draw() = 0;

    void* getNativeWindow();

protected:
    void* m_window;
};
