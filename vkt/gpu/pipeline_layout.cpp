#include "vkt/gpu/pipeline_layout.h"
#include "vkt/gpu/device.h"

namespace vkt
{

BindingLayout::BindingLayout(Device* device, const BindingLayoutDescriptor& descriptor)
    : m_device(device)
{
}

PipelineLayout::PipelineLayout(Device* device, const PipelineLayoutDescriptor& descriptor)
    : m_device(device)
{
}
} // namespace vkt