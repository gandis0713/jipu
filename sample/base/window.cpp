#include "window.h"

#include <SDL2/SDL.h>
#include <SDL_syswm.h>

Window::Window(int w, int h, std::string title)
    : m_window(nullptr)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        return;
    }

    SDL_Window* window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN);
    if (!window)
    {
        SDL_Quit();
        return;
    }

    m_window = static_cast<void*>(window);
}

Window::~Window()
{
    SDL_DestroyWindow(static_cast<SDL_Window*>(m_window));
    SDL_Quit();
}

int Window::exec()
{
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