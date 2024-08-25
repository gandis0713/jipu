#include "vulkan_query_set.h"

#include "vulkan_device.h"

namespace jipu
{

VulkanQuerySet::VulkanQuerySet(VulkanDevice& device, const QuerySetDescriptor& descriptor)
    : m_device(device)
{
    auto& vkAPI = m_device.vkAPI;

    VkQueryPoolCreateInfo queryPoolInfo = {};
    queryPoolInfo.sType = VK_STRUCTURE_TYPE_QUERY_POOL_CREATE_INFO;
    queryPoolInfo.queryType = ToVkQueryType(descriptor.type);
    queryPoolInfo.queryCount = static_cast<uint32_t>(descriptor.count);

    VkQueryPool queryPool;
    VkResult result = vkAPI.CreateQueryPool(m_device.getVkDevice(), &queryPoolInfo, nullptr, &m_queryPool);
    if (result != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create query pool.");
    }
}

VulkanQuerySet::~VulkanQuerySet()
{
    auto& vkAPI = m_device.vkAPI;
    vkAPI.DestroyQueryPool(m_device.getVkDevice(), m_queryPool, nullptr);
}

VkQueryPool VulkanQuerySet::getVkQueryPool() const
{
    return m_queryPool;
}

VkQueryType ToVkQueryType(QueryType type)
{
    switch (type)
    {
    case QueryType::kOcclusion:
        return VK_QUERY_TYPE_OCCLUSION;
    case QueryType::kTimestamp:
        return VK_QUERY_TYPE_TIMESTAMP;
    }
}

} // namespace jipu