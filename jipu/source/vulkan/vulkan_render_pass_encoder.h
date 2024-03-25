#pragma once

#include "jipu/render_pass_encoder.h"
#include "vulkan_api.h"
#include "vulkan_export.h"
#include "vulkan_framebuffer.h"
#include "vulkan_pipeline.h"
#include "vulkan_render_pass.h"

#include "utils/cast.h"

namespace jipu
{

struct VulkanRenderPassEncoderDescriptor
{
    const void* next = nullptr;
    VkRenderPass renderPass = VK_NULL_HANDLE;
    VkFramebuffer framebuffer = VK_NULL_HANDLE;
    VkRect2D renderArea{};
    std::vector<VkClearValue> clearValues{};
};

class VulkanDevice;
class VulkanRenderPass;
class VulkanFramebuffer;
class VulkanCommandBuffer;
class VULKAN_EXPORT VulkanRenderPassEncoder : public RenderPassEncoder
{
public:
    VulkanRenderPassEncoder() = delete;
    VulkanRenderPassEncoder(VulkanCommandBuffer& commandBuffer, const RenderPassEncoderDescriptor& descriptor);
    VulkanRenderPassEncoder(VulkanCommandBuffer& commandBuffer, const VulkanRenderPassEncoderDescriptor& descriptor);
    ~VulkanRenderPassEncoder() override = default;

    void setPipeline(RenderPipeline& pipeline) override;
    void setBindingGroup(uint32_t index, BindingGroup* bindingGroup, std::vector<uint32_t> dynamicOffset = {}) override;
    void setVertexBuffer(uint32_t slot, Buffer* buffer) override;
    void setIndexBuffer(Buffer* buffer, IndexFormat format) override;
    void setViewport(float x,
                     float y,
                     float width,
                     float height,
                     float minDepth,
                     float maxDepth) override;
    void setScissor(float x,
                    float y,
                    float width,
                    float height) override;

    void draw(uint32_t vertexCount) override;
    void drawIndexed(uint32_t indexCount,
                     uint32_t instanceCount,
                     uint32_t indexOffset,
                     uint32_t vertexOffset,
                     uint32_t firstInstance) override;

    void end() override;

public:
    void nextPass();

private:
    void initialize();

private:
    VulkanCommandBuffer& m_commandBuffer;
    std::optional<VulkanRenderPipeline::Ref> m_pipeline = std::nullopt;

    uint32_t m_passIndex = 0;

    const VulkanRenderPassEncoderDescriptor m_descriptor{};
};
DOWN_CAST(VulkanRenderPassEncoder, RenderPassEncoder);

// Generate Helper
VulkanRenderPassDescriptor VULKAN_EXPORT generateVulkanRenderPassDescriptor(const RenderPassEncoderDescriptor& descriptor);
VulkanFramebufferDescriptor VULKAN_EXPORT generateVulkanFramebufferDescriptor(VulkanRenderPass* renderPass, const RenderPassEncoderDescriptor& descriptor);
VulkanRenderPassEncoderDescriptor VULKAN_EXPORT generateVulkanRenderPassEncoderDescriptor(VulkanDevice* device, const RenderPassEncoderDescriptor& descriptor);

// Convert Helper
VkIndexType ToVkIndexType(IndexFormat format);

} // namespace jipu