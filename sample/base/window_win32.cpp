#include "window.h"

#include <SDL2/SDL.h>
#include <SDL_syswm.h>

namespace jipu
{

void* Window::getWindowHandle()
{
    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    SDL_GetWindowWMInfo(static_cast<SDL_Window*>(m_handle), &wmi);
    return static_cast<void*>(wmi.info.win.window);
}

} // namespace jipu