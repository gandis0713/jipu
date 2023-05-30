#include "window.h"

#include <SDL2/SDL.h>
#include <SDL_syswm.h>

void* Window::getNativeWindow()
{
    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    SDL_GetWindowWMInfo(static_cast<SDL_Window*>(m_window), &wmi);
    return static_cast<void*>(wmi.info.win.window);
}
