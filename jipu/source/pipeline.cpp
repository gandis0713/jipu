#include "jipu/pipeline.h"
#include "jipu/device.h"

namespace jipu
{

// Pipeline
Pipeline::Pipeline(Device* device)
    : m_device(device)
{
}

PipelineLayout* Pipeline::getPipelineLayout() const
{
    return m_pipelineLayout;
}

// RenderPipeline
RenderPipeline::RenderPipeline(Device* device, const RenderPipelineDescriptor& descriptor)
    : Pipeline(device)
    , m_descriptor(descriptor)
{
    m_pipelineLayout = m_descriptor.layout;
}

// ComputePipeline
ComputePipeline::ComputePipeline(Device* device, const ComputePipelineDescriptor& descriptor)
    : Pipeline(device)
    , m_descriptor(descriptor)
{
    m_pipelineLayout = m_descriptor.layout;
}

} // namespace jipu
