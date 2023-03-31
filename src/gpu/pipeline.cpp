#include "gpu/pipeline.h"
#include "gpu/device.h"

namespace vkt
{

Pipeline::Pipeline(Device* device, PipelineDescriptor descriptor)
    : m_device(device)
{
}

} // namespace vkt
