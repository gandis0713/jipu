#include "vkt/gpu/pipeline.h"
#include "vkt/gpu/device.h"

namespace vkt
{

Pipeline::Pipeline(Device* device, const PipelineDescriptor& descriptor)
    : m_device(device)
    , m_vertex(descriptor.vertex)
    , m_fragment(descriptor.fragment)
{
}

} // namespace vkt
