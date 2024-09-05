#pragma once

#include <webgpu.h>

#include "common/ref_counted.h"

#include "jipu/surface.h"
#include "jipu/swapchain.h"

namespace jipu
{

class WebGPUDevice;
class WebGPUInstance;
class WebGPUAdapter;
class WebGPUSurface : public RefCounted
{

public:
    static WebGPUSurface* create(WebGPUInstance* instance, WGPUSurfaceDescriptor const* descriptor);

public:
    WebGPUSurface() = delete;
    explicit WebGPUSurface(WebGPUInstance* instance, WGPUSurfaceDescriptor const* descriptor);

public:
    virtual ~WebGPUSurface() = default;

    WebGPUSurface(const WebGPUSurface&) = delete;
    WebGPUSurface& operator=(const WebGPUSurface&) = delete;

public: // WebGPU API
    WGPUStatus getCapabilities(WebGPUAdapter* adapter, WGPUSurfaceCapabilities* capabilities);

public:
    enum class Type
    {
        kUndefined,
        kMetalLayer,
        kWindowsHWND,
        kAndroidWindow,
    };

private:
    [[maybe_unused]] WebGPUInstance* m_wgpuInstance = nullptr;
    [[maybe_unused]] WebGPUDevice* m_wgpuDevice = nullptr;
    [[maybe_unused]] const WGPUSurfaceDescriptor m_descriptor{};

private:
    std::unique_ptr<Surface> m_surface = nullptr;
    std::unique_ptr<Swapchain> m_swapchain = nullptr;

private:
    Type m_type = Type::kUndefined;

    // metal
    void* m_meterLayer = nullptr;

    // android
    void* m_androidNativeWindow = nullptr;

    // windows
    void* m_hInstance = nullptr;
    void* m_HWND = nullptr;
};

// Convert from WebGPU to JIPU
WGPUTextureFormat ToWGPUTextureFormat(TextureFormat format);
WGPUCompositeAlphaMode ToWGPUCompositeAlphaMode(CompositeAlphaFlag flag);

// Convert from JIPU to WebGPU
// TextureFormat ToWGPUTextureFormat(WGPUTextureFormat format);

} // namespace jipu