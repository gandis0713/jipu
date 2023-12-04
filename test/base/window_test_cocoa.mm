#include "window_test.h"

#include <SDL_syswm.h>

#import <Cocoa/Cocoa.h>
#import <QuartzCore/CAMetalLayer.h>

namespace jipu
{

void* WindowTest::handle()
{
    SDL_SysWMinfo wmi;
    SDL_VERSION(&wmi.version);
    SDL_GetWindowWMInfo(m_window, &wmi);
    return [wmi.info.cocoa.window contentView];
}

} // namespace jipu
