#include "jipu/pipeline_layout.h"
#include "jipu/binding_group_layout.h"
#include "jipu/device.h"

namespace jipu
{

PipelineLayout::PipelineLayout(Device* device, const PipelineLayoutDescriptor& descriptor)
    : m_device(device)
{
}
} // namespace jipu