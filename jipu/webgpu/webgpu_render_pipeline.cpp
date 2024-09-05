#include "webgpu_render_pipeline.h"

namespace jipu
{

WebGPURenderPipeline* WebGPURenderPipeline::create(WebGPUDevice* device, WGPURenderPipelineDescriptor const* descriptor)
{
    return nullptr;
}

WebGPURenderPipeline::WebGPURenderPipeline(WebGPUDevice* device, std::unique_ptr<RenderPipeline> pipeline, WGPURenderPipelineDescriptor const* descriptor)
    : m_wgpuDevice(device)
    , m_descriptor(*descriptor)
    , m_pipeline(std::move(pipeline))
{
}

} // namespace jipu