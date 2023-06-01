// Copyright 2016 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#include "VulkanMain.hpp"
#include "vkt/gpu/driver.h"
#include "vkt/gpu/device.h"
#include "vkt/gpu/physical_device.h"
#include "vkt/gpu/texture.h"
#include "vkt/gpu/surface.h"
#include "vkt/gpu/swapchain.h"
#include <android/log.h>
#include <memory>

void InitVKT(android_app* app)
{
    vkt::DriverDescriptor driverDesc{ vkt::DriverType::VULKAN };
    std::unique_ptr<vkt::Driver> driver = vkt::Driver::create(driverDesc);

    vkt::SurfaceDescriptor descriptor{ .windowHandle = static_cast<void*>(app->window) };
    std::unique_ptr<vkt::Surface> surface = driver->createSurface(descriptor);

    vkt::PhysicalDeviceDescriptor physicalDeviceDesc{};
    std::unique_ptr<vkt::PhysicalDevice> physicalDevice = driver->createPhysicalDevice(physicalDeviceDesc);

    vkt::DeviceDescriptor deviceDesk{};
    std::unique_ptr<vkt::Device> device = physicalDevice->createDevice(deviceDesk);

    vkt::SwapchainDescriptor swapchainDesc{ .textureFormat = vkt::TextureFormat::kRGBA_8888_UInt_Norm,
            .presentMode = vkt::PresentMode::kFifo,
            .colorSpace = vkt::ColorSpace::kSRGBNonLinear,
            .width = 800,
            .height = 600,
            .surface = surface.get() };
    std::unique_ptr<vkt::Swapchain> swapchain = device->createSwapchain(swapchainDesc);
}

// Process the next main command.
void handle_cmd(android_app* app, int32_t cmd)
{
    switch (cmd)
    {
    case APP_CMD_INIT_WINDOW:
        InitVKT(app);
        // The window is being shown, get it ready.
        InitVulkan(app);
        break;
    case APP_CMD_TERM_WINDOW:
        // The window is being hidden or closed, clean it up.
        DeleteVulkan();
        break;
    default:
        __android_log_print(ANDROID_LOG_INFO, "Vulkan Tutorials",
                            "event not handled: %d", cmd);
    }
}

void android_main(struct android_app* app)
{

    // Set the callback to process system events
    app->onAppCmd = handle_cmd;

    // Used to poll the events in the main loop
    int events;
    android_poll_source* source;

    // Main loop
    do
    {
        if (ALooper_pollAll(IsVulkanReady() ? 1 : 0, nullptr,
                            &events, (void**)&source) >= 0)
        {
            if (source != NULL)
                source->process(app, source);
        }

        // render if vulkan is ready
        if (IsVulkanReady())
        {
            VulkanDrawFrame();
        }
    } while (app->destroyRequested == 0);
}
