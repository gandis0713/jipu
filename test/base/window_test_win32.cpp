#include "window_test.h"

#include <SDL_syswm.h>

void* WindowTest::handle()
{
    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    SDL_GetWindowWMInfo(m_window, &wmi);
    return static_cast<void*>(wmi.info.win.window);
}