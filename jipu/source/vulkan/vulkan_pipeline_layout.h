#pragma once

#include "export.h"

#include "jipu/pipeline_layout.h"
#include "utils/cast.h"
#include "vulkan_api.h"

namespace jipu
{

class VulkanDevice;
class JIPU_EXPERIMENTAL_EXPORT VulkanPipelineLayout : public PipelineLayout
{
public:
    VulkanPipelineLayout() = delete;
    VulkanPipelineLayout(VulkanDevice* device, const PipelineLayoutDescriptor& descriptor);
    ~VulkanPipelineLayout() override;

public:
    VkPipelineLayout getVkPipelineLayout() const;

private:
    VulkanDevice* m_device = nullptr;

private:
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
};

DOWN_CAST(VulkanPipelineLayout, PipelineLayout);

} // namespace jipu