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

// Vulkan Render Pipeline
struct VulkanRenderPipelineDescriptor : RenderPipelineDescriptor
{
    VkRenderPass renderPass = VK_NULL_HANDLE;
    uint32_t subpassIndex = 0;
};

class VulkanRenderPass;
class JIPU_EXPERIMENTAL_EXPORT VulkanRenderPipeline : public RenderPipeline
{
public:
    VulkanRenderPipeline() = delete;
    VulkanRenderPipeline(VulkanDevice* device, const VulkanRenderPipelineDescriptor& descriptor);
    ~VulkanRenderPipeline() override;

    VulkanRenderPipeline(const VulkanRenderPipeline&) = delete;
    VulkanRenderPipeline& operator=(const VulkanRenderPipeline&) = delete;

public:
    PipelineLayout* getPipelineLayout() const override;

public:
    VkPipeline getVkPipeline() const;

private:
    void initialize();

private:
    VulkanDevice* m_device = nullptr;
    const VulkanRenderPipelineDescriptor m_descriptor{};

private:
    VkPipeline m_pipeline = VK_NULL_HANDLE;
};
DOWN_CAST(VulkanRenderPipeline, RenderPipeline);

// Generator Helper
VulkanRenderPassDescriptor generateVulkanRenderPassDescriptor(const RenderPipelineDescriptor& descriptor);

// Convert Helper
VkFormat ToVkVertexFormat(VertexFormat format);
VkPrimitiveTopology ToVkPrimitiveTopology(PrimitiveTopology topology);
VkCullModeFlags ToVkCullModeFlags(CullMode mode);
VkFrontFace ToVkFrontFace(FrontFace frontFace);
VkBlendOp ToVkBlendOp(BlendOperation op);
VkBlendFactor ToVkBlendFactor(BlendFactor factor);
VkVertexInputRate ToVkVertexInputRate(VertexMode mode);

} // namespace jipu
