#include "window_test.h"

void WindowTest::SetUp()
{
    EXPECT_GT(SDL_Init(SDL_INIT_VIDEO), -1);
    m_window = SDL_CreateWindow("Window Test",
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                1280,
                                720,
                                SDL_WINDOW_SHOWN);
    EXPECT_NE(nullptr, m_window);
}

void WindowTest::TearDown()
{
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}