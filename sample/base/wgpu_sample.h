#pragma once

#include "window.h"

#include <filesystem>
#include <webgpu.h>

namespace jipu
{

struct WGPUSampleDescriptor
{
    WindowDescriptor windowDescriptor;
    std::filesystem::path path;
};

class WGPUSample : public Window
{
public:
    WGPUSample() = delete;
    WGPUSample(const WGPUSampleDescriptor& descriptor);
    virtual ~WGPUSample();

    struct WGPUContext
    {
        WGPUInstance instance = nullptr;
        WGPUAdapter adapter = nullptr;
        WGPUDevice device = nullptr;
        WGPUSurface surface = nullptr;
        WGPUQueue queue = nullptr;
        WGPUSwapChain swapchain = nullptr;

        WGPUSurfaceCapabilities surfaceCapabilities{};
        WGPUSurfaceConfiguration surfaceConfigure{};
    };

public:
    virtual void createInstance();
    virtual void createSurface();
    virtual void createAdapter();
    virtual void createDevice();
    virtual void createSurfaceConfigure();
    virtual void createQueue();

public:
    void init() override;
    void update() override;

protected:
    std::filesystem::path m_appPath;
    std::filesystem::path m_appDir;

    WGPUContext m_wgpuContext{};
};

} // namespace jipu
