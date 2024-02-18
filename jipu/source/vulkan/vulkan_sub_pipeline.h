#pragma once

#include "vulkan_pipeline.h"

namespace jipu
{

struct VulkanSubPipelineDescriptor : RenderPipelineDescriptor
{
    uint32_t subpassIndex = 0;
};

class VulkanDevice;
class VulkanSubPipeline
{
public:
    VulkanSubPipeline() = delete;
    VulkanSubPipeline(VulkanDevice* device, const VulkanSubPipelineDescriptor& descriptor);
    ~VulkanSubPipeline() = default;

    VulkanSubPipeline(const VulkanSubPipeline&) = delete;
    VulkanSubPipeline& operator=(const VulkanSubPipeline&) = delete;

private:
    VulkanDevice* m_device = nullptr;
    const VulkanSubPipelineDescriptor m_descriptor{};
};

} // namespace jipu