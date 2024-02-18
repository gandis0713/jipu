#pragma once

#include "vulkan_pipeline_layout.h"

namespace jipu
{

struct VulkanSubPipelineLayoutDescriptor : PipelineLayoutDescriptor
{
};

class VulkanDevice;
class VulkanSubPipelineLayout
{
public:
    VulkanSubPipelineLayout() = delete;
    VulkanSubPipelineLayout(VulkanDevice* device, const VulkanSubPipelineLayoutDescriptor& descriptor);
    ~VulkanSubPipelineLayout();

    VulkanSubPipelineLayout(const VulkanSubPipelineLayout&) = delete;
    VulkanSubPipelineLayout operator=(const VulkanSubPipelineLayout&) = delete;

private:
    VulkanDevice* m_device = nullptr;
    const VulkanSubPipelineLayoutDescriptor m_descriptor{};
};

} // namespace jipu