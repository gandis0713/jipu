#pragma once

#include "jipu/pipeline.h"
#include "utils/cast.h"
#include "vulkan_api.h"
#include "vulkan_shader_module.h"

#include <string>
#include <vector>

namespace jipu
{

class VulkanDevice;

// Vulkan Compute Pipeline
class VulkanComputePipeline : public ComputePipeline
{
public:
    VulkanComputePipeline() = delete;
    VulkanComputePipeline(VulkanDevice* device, const ComputePipelineDescriptor& descriptor);
    ~VulkanComputePipeline() override;

    VulkanComputePipeline(const VulkanComputePipeline&) = delete;
    VulkanComputePipeline& operator=(const VulkanComputePipeline&) = delete;

    VkPipeline getVkPipeline() const;

private:
    void initialize();

private:
    VkPipeline m_pipeline = VK_NULL_HANDLE;
};
DOWN_CAST(VulkanComputePipeline, ComputePipeline);

class VulkanRenderPass;

// Vulkan Render Pipeline
class VulkanRenderPipeline : public RenderPipeline
{
public:
    VulkanRenderPipeline() = delete;
    VulkanRenderPipeline(VulkanDevice* device, const RenderPipelineDescriptor& descriptor);
    ~VulkanRenderPipeline() override;

    VulkanRenderPipeline(const VulkanRenderPipeline&) = delete;
    VulkanRenderPipeline& operator=(const VulkanRenderPipeline&) = delete;

    VkPipeline getVkPipeline() const;

private:
    void initialize();

private:
    VkPipeline m_pipeline = VK_NULL_HANDLE;
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
