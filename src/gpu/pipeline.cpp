#include "vkt/gpu/pipeline.h"
#include "vkt/gpu/device.h"

namespace vkt
{

Pipeline::Pipeline(Device* device, const PipelineDescriptor& descriptor)
    : m_device(device)
{
}

} // namespace vkt
