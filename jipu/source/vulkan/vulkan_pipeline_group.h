#pragma once

#include "jipu/pipeline.h"

#include "export.h"

#include "vulkan_pipeline.h"

namespace jipu
{

struct VulkanRenderPipelineGroupDescriptor
{
    std::vector<RenderPipelineDescriptor> pipelines{};
};

class VulkanDevice;
class JIPU_EXPERIMENTAL_EXPORT VulkanRenderPipelineGroup
{
public:
    VulkanRenderPipelineGroup() = delete;
    VulkanRenderPipelineGroup(VulkanDevice* device, const VulkanRenderPipelineGroupDescriptor& descriptor);
    ~VulkanRenderPipelineGroup() = default;

    VulkanRenderPipelineGroup(const VulkanRenderPipelineGroup&) = delete;
    VulkanRenderPipelineGroup& operator=(const VulkanRenderPipelineGroup&) = delete;

public:
    VulkanRenderPipeline* getRenderPipeline(uint32_t index) const;

private:
    VulkanDevice* m_device = nullptr;
    const VulkanRenderPipelineGroupDescriptor m_descriptor{};

private:
    std::vector<std::unique_ptr<RenderPipeline>> m_pipelines{};
};

// Generator Helper
std::vector<VulkanRenderPassDescriptor> generateVulkanRenderPassDescriptor(const VulkanRenderPipelineGroupDescriptor& descriptor);

} // namespace jipu