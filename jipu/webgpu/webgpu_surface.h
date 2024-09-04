#pragma once

#include <webgpu.h>

#include "common/ref_counted.h"

#include "jipu/surface.h"
#include "jipu/swapchain.h"

namespace jipu
{

class WebGPUAdapter;
class WebGPUInstance;
class WebGPUSurface : public RefCounted
{
public:
    WebGPUSurface() = delete;
    explicit WebGPUSurface(WebGPUInstance* instance, WGPUSurfaceDescriptor const* descriptor);

public:
    virtual ~WebGPUSurface() = default;

    WebGPUSurface(const WebGPUSurface&) = delete;
    WebGPUSurface& operator=(const WebGPUSurface&) = delete;

private:
    [[maybe_unused]] WebGPUInstance* m_wgpuInstance = nullptr;
    [[maybe_unused]] WebGPUAdapter* m_wgpuAdapter = nullptr;

private:
    std::unique_ptr<Surface> m_surface = nullptr;
    std::unique_ptr<Swapchain> m_swapchain = nullptr;
};

} // namespace jipu