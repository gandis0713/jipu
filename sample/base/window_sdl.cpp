#include "window.h"

#include <SDL2/SDL.h>
#include <SDL_syswm.h>

namespace vkt
{

Window::Window(const WindowDescriptor& descriptor)
    : m_handle(descriptor.handle)
    , m_width(descriptor.width)
    , m_height(descriptor.height)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        return;
    }

    SDL_Window* window = SDL_CreateWindow(descriptor.title.c_str(),
                                          SDL_WINDOWPOS_UNDEFINED,
                                          SDL_WINDOWPOS_UNDEFINED,
                                          descriptor.width,
                                          descriptor.height,
                                          SDL_WINDOW_SHOWN);
    if (!window)
    {
        SDL_Quit();
        return;
    }

    m_handle = static_cast<void*>(window);
}

Window::~Window()
{
    SDL_DestroyWindow(static_cast<SDL_Window*>(m_handle));
    SDL_Quit();
}

int Window::exec()
{
    init();

    SDL_Event event;
    int quit = 0;
    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_QUIT)
            {
                quit = 1;
            }
        }

        draw();
    }

    return 0;
}

} // namespace vkt