#include "jipu/gpu/pipeline_layout.h"
#include "jipu/gpu/binding_group_layout.h"
#include "jipu/gpu/device.h"

namespace jipu
{

PipelineLayout::PipelineLayout(Device* device, const PipelineLayoutDescriptor& descriptor)
    : m_device(device)
{
}
} // namespace jipu