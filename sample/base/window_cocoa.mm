#include "window.h"

#include <SDL2/SDL.h>
#include <SDL_syswm.h>

#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>

void* Window::getNativeWindow()
{
    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    SDL_GetWindowWMInfo(static_cast<SDL_Window*>(m_window), &wmi);
    return [wmi.info.cocoa.window contentView];
}
