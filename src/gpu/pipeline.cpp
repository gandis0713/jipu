#include "gpu/pipeline.h"
#include "gpu/device.h"

namespace vkt
{

Pipeline::Pipeline(Device* device, const PipelineDescriptor& descriptor)
    : m_device(device)
{
}

} // namespace vkt
