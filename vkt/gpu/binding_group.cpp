#include "vkt/gpu/binding_group.h"

#include "vkt/gpu/binding_group_layout.h"
#include "vkt/gpu/device.h"

namespace vkt
{

BindingGroup::BindingGroup(Device* device, const BindingGroupDescriptor& descriptor)
    : m_device(device)
{
}

} // namespace vkt