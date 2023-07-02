#include "vkt/gpu/binding_group_layout.h"

#include "vkt/gpu/device.h"

namespace vkt
{

BindingGroupLayout::BindingGroupLayout(Device* device, const BindingGroupLayoutDescriptor& descriptor)
    : m_device(device)
{
}

} // namespace vkt