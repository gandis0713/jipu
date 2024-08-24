#pragma once

#include "jipu/query_set.h"
#include "utils/cast.h"
#include "vulkan_api.h"
#include "vulkan_export.h"

namespace jipu
{

class VulkanDevice;
class VULKAN_EXPORT VulkanQuerySet : public QuerySet
{
public:
    VulkanQuerySet() = delete;
    VulkanQuerySet(VulkanDevice& device, const QuerySetDescriptor& descriptor);
    ~VulkanQuerySet() override = default;

private:
    VulkanDevice& m_device;
};

} // namespace jipu