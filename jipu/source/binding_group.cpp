#include "jipu/binding_group.h"

#include "jipu/binding_group_layout.h"
#include "jipu/device.h"
#include "jipu/sampler.h"
#include "jipu/texture_view.h"

#include <fmt/format.h>
#include <stdexcept>

namespace jipu
{

BindingGroup::BindingGroup(Device* device, const BindingGroupDescriptor& descriptor)
{
}

} // namespace jipu