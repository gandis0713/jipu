#pragma once

#include "jipu/gpu/pipeline_layout.h"
#include "utils/cast.h"
#include "vulkan_api.h"

namespace jipu
{

class VulkanDevice;
class VulkanPipelineLayout : public PipelineLayout
{
public:
    VulkanPipelineLayout() = delete;
    VulkanPipelineLayout(VulkanDevice* device, const PipelineLayoutDescriptor& descriptor);
    ~VulkanPipelineLayout() override;

    VkPipelineLayout getVkPipelineLayout() const;

private:
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
};

DOWN_CAST(VulkanPipelineLayout, PipelineLayout);

} // namespace jipu