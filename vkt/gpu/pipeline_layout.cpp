#include "vkt/gpu/pipeline_layout.h"
#include "vkt/gpu/device.h"

namespace vkt
{
PipelineLayout::PipelineLayout(Device* device, const PipelineLayoutDescriptor& descriptor)
    : m_device(device)
{
}
} // namespace vkt