#pragma once

#include "export.h"

#include "jipu/render_pass_encoder.h"
#include "vulkan_api.h"

namespace jipu
{

class VulkanDevice;
class VulkanRenderPass;
class VulkanFramebuffer;
class VulkanCommandBuffer;
class JIPU_EXPERIMENTAL_EXPORT VulkanRenderPassEncoder : public RenderPassEncoder
{
public:
    VulkanRenderPassEncoder() = delete;
    VulkanRenderPassEncoder(VulkanCommandBuffer* commandBuffer, const RenderPassDescriptor& descriptor);
    VulkanRenderPassEncoder(VulkanCommandBuffer* commandBuffer, const std::vector<RenderPassDescriptor>& descriptors);
    ~VulkanRenderPassEncoder() override = default;

    void setPipeline(Pipeline* pipeline) override;
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
};

// Generate Helper
VulkanRenderPass* JIPU_EXPERIMENTAL_EXPORT getVulkanRenderPass(VulkanDevice*, const RenderPassEncoderDescriptor&);
VulkanFramebuffer* JIPU_EXPERIMENTAL_EXPORT getVulkanFramebuffer(VulkanDevice*, VulkanRenderPass*, const RenderPassEncoderDescriptor&);

// Convert Helper
VkIndexType ToVkIndexType(IndexFormat format);

} // namespace jipu