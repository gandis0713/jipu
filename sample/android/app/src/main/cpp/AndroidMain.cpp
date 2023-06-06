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
#include "vkt/gpu/device.h"
#include "vkt/gpu/driver.h"
#include "vkt/gpu/physical_device.h"
#include "vkt/gpu/surface.h"
#include "vkt/gpu/swapchain.h"
#include "vkt/gpu/texture.h"
#include <android/log.h>
#include <memory>
#include <vector>
#include <cassert>

std::vector<char> readFile(android_app* app, const char* filePath) {
    // Read the file
    assert(app);
    AAsset* file = AAssetManager_open(app->activity->assetManager,
                                      filePath, AASSET_MODE_BUFFER);
    size_t fileLength = AAsset_getLength(file);

    std::vector<char> fileContent{};
    fileContent.resize(fileLength);

    AAsset_read(file, fileContent.data(), fileLength);
    AAsset_close(file);

    return fileContent;
}

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

    // queue
    vkt::QueueDescriptor queueDesc{ .flags = vkt::QueueFlagBits::kGraphics };
    std::unique_ptr<vkt::Queue> renderQueue = device->createQueue(queueDesc);

    // create vertex buffer.
    struct Vertex
    {
        float pos[2];
        float color[3];
    };

    {
        uint64_t vertexCount = 4;
        Vertex vertices[4] = {
            { { -0.5f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
            { { 0.5f, -0.5f }, { 0.0f, 1.0f, 0.0f } },
            { { 0.5f, 0.5f }, { 0.0f, 0.0f, 1.0f } },
            { { -0.5f, 0.5f }, { 1.0f, 1.0f, 1.0f } }
        };

        uint64_t vertexSize = static_cast<uint64_t>(sizeof(Vertex) * vertexCount);
        vkt::BufferDescriptor vertexBufferDescriptor{ .size = vertexSize,
                                                      .usage = vkt::BufferUsageFlagBits::kVertex };
        std::unique_ptr<vkt::Buffer> vertexBuffer = device->createBuffer(vertexBufferDescriptor);

        void* mappedPointer = vertexBuffer->map();
        memcpy(mappedPointer, &vertices[0], vertexSize);
        vertexBuffer->unmap();

        // index buffer
        uint64_t indexCount = 6;
        float indices[6] = {
            0, 1, 2, 2, 3, 0
        };

        uint64_t indexSize = static_cast<uint64_t>(sizeof(uint64_t) * indexCount);
        vkt::BufferDescriptor indexBufferDescriptor{ .size = indexSize,
                                                     .usage = vkt::BufferUsageFlagBits::kIndex };

        std::unique_ptr<vkt::Buffer> indexBuffer = device->createBuffer(indexBufferDescriptor);

        mappedPointer = indexBuffer->map();
        memcpy(mappedPointer, &indices[0], indexSize);
        indexBuffer->unmap();
    }

    // pipeline
    {
        // Input Assembly
        vkt::InputAssemblyStage inputAssembly{};
        {
            inputAssembly.topology = vkt::PrimitiveTopology::kTriangleList;
        }

        // vertex stage
        vkt::VertexStage vertexStage{};
        {
            // create vertex shader
            const std::vector<char> vertShaderCode = readFile(app, "shaders/triangle.vert.spv");
            vkt::ShaderModuleDescriptor vertexShaderModuleDescriptor{ .code = vertShaderCode.data(),
                                                                      .codeSize = vertShaderCode.size() };
            std::unique_ptr<vkt::ShaderModule> vertexShaderModule = device->createShaderModule(vertexShaderModuleDescriptor);
            vertexStage.shader = vertexShaderModule.get();

            // layouts
            std::vector<vkt::VertexBindingLayout> layouts{};
            layouts.resize(1);
            {
                // attributes
                std::vector<vkt::VertexAttribute> vertexAttributes{};
                vertexAttributes.resize(2);
                {
                    // position
                    vertexAttributes[0] = { .format = vkt::VertexFormat::kSFLOATx2,
                                            .offset = offsetof(Vertex, pos) };

                    // color
                    vertexAttributes[1] = { .format = vkt::VertexFormat::kSFLOATx3,
                                            .offset = offsetof(Vertex, color) };
                }

                vkt::VertexBindingLayout vertexLayout{ .mode = vkt::VertexMode::kVertex,
                                                       .stride = sizeof(Vertex),
                                                       .attributes = vertexAttributes };
                layouts[0] = vertexLayout;
            }

            vertexStage.layouts = layouts;
        }

        // Rasterization
        vkt::RasterizationStage rasterization{};
        {
        }

        // fragment stage
        vkt::FragmentStage fragmentStage{};
        {
            // create fragment shader
            const std::vector<char> fragShaderCode = readFile(app, "shaders/triangle.frag.spv");
            vkt::ShaderModuleDescriptor fragmentShaderModuleDescriptor{ .code = fragShaderCode.data(),
                                                                        .codeSize = fragShaderCode.size() };
            std::unique_ptr<vkt::ShaderModule> fragmentShaderModule = device->createShaderModule(fragmentShaderModuleDescriptor);

            fragmentStage.shader = fragmentShaderModule.get();

            // output targets
            fragmentStage.targets = { { .format = swapchain->getTextureFormat() } };
        }

        vkt::RenderPipelineDescriptor descriptor{ .inputAssembly = inputAssembly,
                                                  .vertex = vertexStage,
                                                  .rasterization = rasterization,
                                                  .fragment = fragmentStage };

        std::unique_ptr<vkt::RenderPipeline> renderPipeline = device->createRenderPipeline(descriptor);
    }
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
