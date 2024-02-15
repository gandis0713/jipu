#pragma once

#include "export.h"

#include "jipu/pipeline.h"
#include "utils/cast.h"
#include "vulkan_api.h"
#include "vulkan_render_pass.h"
#include "vulkan_shader_module.h"

#include <string>
#include <vector>

namespace jipu
{

class VulkanDevice;
class VulkanPipelineLayout;
class JIPU_EXPERIMENTAL_EXPORT VulkanComputePipeline : public ComputePipeline
{
public:
    VulkanComputePipeline() = delete;
    VulkanComputePipeline(VulkanDevice* device, const ComputePipelineDescriptor& descriptor);
    ~VulkanComputePipeline() override;

    VulkanComputePipeline(const VulkanComputePipeline&) = delete;
    VulkanComputePipeline& operator=(const VulkanComputePipeline&) = delete;

public:
    PipelineLayout* getPipelineLayout() const override;

public:
    VkPipeline getVkPipeline() const;

private:
    void initialize();

private:
    VulkanDevice* m_device = nullptr;

    const ComputePipelineDescriptor m_descriptor{};

private:
    VkPipeline m_pipeline = VK_NULL_HANDLE;
};
DOWN_CAST(VulkanComputePipeline, ComputePipeline);

class VulkanRenderPass;

// Vulkan Render Pipeline
class JIPU_EXPERIMENTAL_EXPORT VulkanRenderPipeline : public RenderPipeline
{
public:
    VulkanRenderPipeline() = delete;
    VulkanRenderPipeline(VulkanDevice* device, const RenderPipelineDescriptor& descriptor);
    VulkanRenderPipeline(VulkanDevice* device, const std::vector<RenderPipelineDescriptor>& descriptors);
    ~VulkanRenderPipeline() override;

    VulkanRenderPipeline(const VulkanRenderPipeline&) = delete;
    VulkanRenderPipeline& operator=(const VulkanRenderPipeline&) = delete;

public:
    PipelineLayout* getPipelineLayout() const override;

public:
    PipelineLayout* getPipelineLayout(uint32_t index) const;

    VkPipeline getVkPipeline(uint32_t index = 0) const;

private:
    void initialize(const std::vector<RenderPipelineDescriptor>& descriptors);

private:
    VulkanDevice* m_device = nullptr;

    const std::vector<RenderPipelineDescriptor> m_descriptors{};

private:
    std::vector<VkPipeline> m_pipelines{};
};
DOWN_CAST(VulkanRenderPipeline, RenderPipeline);

// Convert Helper
VkFormat ToVkVertexFormat(VertexFormat format);
VkPrimitiveTopology ToVkPrimitiveTopology(PrimitiveTopology topology);
VkCullModeFlags ToVkCullModeFlags(CullMode mode);
VkFrontFace ToVkFrontFace(FrontFace frontFace);
VkBlendOp ToVkBlendOp(BlendOperation op);
VkBlendFactor ToVkBlendFactor(BlendFactor factor);
VkVertexInputRate ToVkVertexInputRate(VertexMode mode);

} // namespace jipu
