#pragma once

#include <SDL2/SDL.h>
#include <gtest/gtest.h>

class WindowTest : public testing::Test
{
protected:
    void SetUp() override;
    void TearDown() override;

    void* handle();

    SDL_Window* m_window;
};