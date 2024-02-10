#include "jipu/pipeline.h"
#include "jipu/device.h"

namespace jipu
{

// Pipeline
Pipeline::Pipeline(Device* device)
    : m_device(device)
{
}



// RenderPipeline
RenderPipeline::RenderPipeline(Device* device, const RenderPipelineDescriptor& descriptor)
    : Pipeline(device)
    , m_descriptors{descriptor}
{
}

RenderPipeline::RenderPipeline(Device* device, const std::vector<RenderPipelineDescriptor>& descriptors)
    : Pipeline(device)
    , m_descriptors(descriptors)
{
}

PipelineLayout* RenderPipeline::getPipelineLayout(uint32_t index) const
{
    return m_descriptors[index].layout;
}

// ComputePipeline
ComputePipeline::ComputePipeline(Device* device, const ComputePipelineDescriptor& descriptor)
    : Pipeline(device)
    , m_descriptor(descriptor)
{
}

PipelineLayout* ComputePipeline::getPipelineLayout(uint32_t index) const
{
    return m_descriptor.layout;
}

} // namespace jipu
