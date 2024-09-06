
#pragma once

#include "common/ref_counted.h"
#include "jipu/render_pass_encoder.h"
#include "webgpu_header.h"

#include <memory>

namespace jipu
{

class WebGPUCommandEncoder;
class WebGPURenderPassEncoder : public RefCounted
{

public:
    static WebGPURenderPassEncoder* create(WebGPUCommandEncoder* wgpuCommandEncoder, WGPURenderPassDescriptor const* descriptor);

public:
    WebGPURenderPassEncoder() = delete;
    explicit WebGPURenderPassEncoder(WebGPUCommandEncoder* wgpuCommandEncoder, std::unique_ptr<RenderPassEncoder> renderPassEncoder, WGPURenderPassDescriptor const* descriptor);

public:
    virtual ~WebGPURenderPassEncoder() = default;

    WebGPURenderPassEncoder(const WebGPURenderPassEncoder&) = delete;
    WebGPURenderPassEncoder& operator=(const WebGPURenderPassEncoder&) = delete;

public: // WebGPU API
public:
    RenderPassEncoder* getRenderPassEncoder() const;

private:
    [[maybe_unused]] WebGPUCommandEncoder* m_wgpuCommandEncoder = nullptr;
    [[maybe_unused]] const WGPURenderPassDescriptor m_descriptor{};

private:
    std::unique_ptr<RenderPassEncoder> m_renderPassEncoder = nullptr;
};

} // namespace jipu