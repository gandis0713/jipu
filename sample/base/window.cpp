#include "window.h"

void Window::setDrawCB(std::function<void()> f)
{
    onDraw = f;
}