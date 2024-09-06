#include "webgpu_render_pass_encoder.h"

namespace jipu
{

WebGPURenderPassEncoder* WebGPURenderPassEncoder::create(WebGPUCommandEncoder* wgpuCommandEncoder, WGPURenderPassDescriptor const* descriptor)
{
    return new WebGPURenderPassEncoder(wgpuCommandEncoder, nullptr, descriptor);
}

WebGPURenderPassEncoder::WebGPURenderPassEncoder(WebGPUCommandEncoder* wgpuCommandEncoder, std::unique_ptr<RenderPassEncoder> renderPassEncoder, WGPURenderPassDescriptor const* descriptor)
    : m_wgpuCommandEncoder(wgpuCommandEncoder)
    , m_descriptor(*descriptor)
    , m_renderPassEncoder(std::move(renderPassEncoder))
{
}

RenderPassEncoder* WebGPURenderPassEncoder::getRenderPassEncoder() const
{
    return m_renderPassEncoder.get();
}

} // namespace jipu