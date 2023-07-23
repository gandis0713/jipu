#include "vkt/gpu/binding_group.h"

#include "vkt/gpu/binding_group_layout.h"
#include "vkt/gpu/device.h"
#include "vkt/gpu/sampler.h"
#include "vkt/gpu/texture_view.h"

namespace vkt
{

BindingGroup::BindingGroup(Device* device, const BindingGroupDescriptor& descriptor)
    : m_device(device)
{
}

} // namespace vkt