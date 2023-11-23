#include "window.h"

#include <SDL2/SDL.h>
#include <SDL_syswm.h>

#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>

namespace jipu
{

void* Window::getWindowHandle()
{
    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    SDL_GetWindowWMInfo(static_cast<SDL_Window*>(m_handle), &wmi);
    return [wmi.info.cocoa.window contentView];
}

} // namespace jipu
