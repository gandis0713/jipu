#include "vulkan_query_set.h"

#include "vulkan_device.h"

namespace jipu
{

VulkanQuerySet::VulkanQuerySet(VulkanDevice& device, const QuerySetDescriptor& descriptor)
    : m_device(device)
{
}

} // namespace jipu