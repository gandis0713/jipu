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
    ~VulkanQuerySet() override;

private:
    VkQueryPool getVkQueryPool() const;

private:
    VulkanDevice& m_device;

private:
    VkQueryPool m_queryPool = VK_NULL_HANDLE;
};
DOWN_CAST(VulkanQuerySet, QuerySet);

// Convert Helper
VkQueryType ToVkQueryType(QueryType type);

} // namespace jipu