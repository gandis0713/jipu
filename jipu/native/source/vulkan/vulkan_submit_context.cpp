#include "vulkan_submit_context.h"

namespace jipu
{

VulkanSubmitContext VulkanSubmitContext::create(const std::vector<VulkanCommandRecordResult>& results)
{
    VulkanSubmitContext context;

    return context;
}

std::vector<VulkanSubmitInfo> VulkanSubmitContext::getSubmitInfos() const
{
    return m_submitInfos;
}

} // namespace jipu