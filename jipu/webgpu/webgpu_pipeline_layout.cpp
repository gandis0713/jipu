#include "webgpu_pipeline_layout.h"

namespace jipu
{

WebGPUPipelineLayout* WebGPUPipelineLayout::create(WebGPUDevice* device, WGPUPipelineLayoutDescriptor const* descriptor)
{
    // for(auto i = 0; i < descriptor->bindGroupLayoutCount; ++i)
    // {
    //     descriptor->bindGroupLayouts[i];
    // }
    // PipelineLayoutDescriptor pipelineLayoutDescriptor{};
    // pipelineLayoutDescriptor.layouts
    // return new WebGPUPipelineLayout(device, std::make_unique<PipelineLayout>(), descriptor);

    return nullptr;
}

WebGPUPipelineLayout::WebGPUPipelineLayout(WebGPUDevice* device, std::unique_ptr<PipelineLayout> layout, WGPUPipelineLayoutDescriptor const* descriptor)
    : m_wgpuDevice(device)
    , m_descriptor(*descriptor)
    , m_layout(std::move(layout))
{
}

} // namespace jipu