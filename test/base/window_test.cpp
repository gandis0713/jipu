#include "window_test.h"

namespace jipu
{

void WindowTest::SetUp()
{
    Test::SetUp();

    m_width = 1280;
    m_height = 720;
    EXPECT_GT(SDL_Init(SDL_INIT_VIDEO), -1);
    m_window = SDL_CreateWindow("Window Test",
                                SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED,
                                m_width,
                                m_height,
                                SDL_WINDOW_SHOWN);
    EXPECT_NE(nullptr, m_window);

    SurfaceDescriptor surfaceDescriptor{ .windowHandle = handle() };
    m_surface = m_driver->createSurface(surfaceDescriptor);
    EXPECT_NE(nullptr, m_surface);

#if defined(__ANDROID__) || defined(ANDROID)
    TextureFormat textureFormat = TextureFormat::kRGBA_8888_UInt_Norm_SRGB;
#else
    TextureFormat textureFormat = TextureFormat::kBGRA_8888_UInt_Norm_SRGB;
#endif
    SwapchainDescriptor descriptor{
        *m_surface,
        .textureFormat = textureFormat,
        .presentMode = PresentMode::kFifo,
        .colorSpace = ColorSpace::kSRGBNonLinear,
        .width = m_width,
        .height = m_height
    };

    m_swapchain = m_device->createSwapchain(descriptor);
    EXPECT_NE(nullptr, m_swapchain);
}

void WindowTest::TearDown()
{
    m_swapchain.reset();
    m_surface.reset();

    SDL_DestroyWindow(m_window);
    SDL_Quit();

    Test::TearDown();
}

} // namespace jipu