#include "vkt/gpu/pipeline_layout.h"
#include "vkt/gpu/binding_group_layout.h"
#include "vkt/gpu/device.h"

namespace jipu
{

PipelineLayout::PipelineLayout(Device* device, const PipelineLayoutDescriptor& descriptor)
    : m_device(device)
{
}
} // namespace jipu