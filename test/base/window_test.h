#pragma once

#include <SDL2/SDL.h>
#include <gtest/gtest.h>

#include "jipu/device.h"
#include "jipu/driver.h"
#include "jipu/physical_device.h"
#include "jipu/surface.h"
#include "jipu/swapchain.h"

class WindowTest : public testing::Test
{
protected:
    void SetUp() override;
    void TearDown() override;

    void* handle();

    SDL_Window* m_window;
    uint32_t m_width = 0;
    uint32_t m_height = 0;

    std::unique_ptr<jipu::Driver> m_driver = nullptr;
    std::vector<std::unique_ptr<jipu::PhysicalDevice>> m_physicalDevices{};
    std::unique_ptr<jipu::Device> m_device = nullptr;
    std::unique_ptr<jipu::Surface> m_surface = nullptr;
    std::unique_ptr<jipu::Swapchain> m_swapchain = nullptr;
};