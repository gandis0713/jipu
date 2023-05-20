#include "vkt/gpu/pipeline.h"
#include "vkt/gpu/device.h"

namespace vkt
{

/* Pipeline */

Pipeline::Pipeline(Device* device)
    : m_device(device)
{
}

/* RenderPipeline */

RenderPipeline::RenderPipeline(Device* device, const RenderPipelineDescriptor& descriptor)
    : Pipeline(device)
    , m_vertex(descriptor.vertex)
    , m_fragment(descriptor.fragment)
{
}

} // namespace vkt
