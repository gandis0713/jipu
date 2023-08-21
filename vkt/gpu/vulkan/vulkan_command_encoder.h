#pragma once

#include "vkt/gpu/command_encoder.h"
#include "vulkan_api.h"

namespace vkt
{

class VulkanCommandBuffer;
class VulkanRenderPassEncoder : public RenderPassEncoder
{
public:
    VulkanRenderPassEncoder() = delete;
    VulkanRenderPassEncoder(VulkanCommandBuffer* commandBuffer, const RenderPassEncoderDescriptor& descriptor);
    ~VulkanRenderPassEncoder() override = default;

    void setPipeline(Pipeline* pipeline) override;
    void setBindingGroup(uint32_t index, BindingGroup* bindingGroup) override;
    void setVertexBuffer(Buffer* buffer) override;
    void setIndexBuffer(Buffer* buffer) override;
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
    void drawIndexed(uint32_t indexCount) override;

    void end() override;
};

class VulkanCommandEncoder : public CommandEncoder
{
public:
    VulkanCommandEncoder() = delete;
    VulkanCommandEncoder(VulkanCommandBuffer* commandBuffer, const CommandEncoderDescriptor& descriptor);
    ~VulkanCommandEncoder() override = default;

    std::unique_ptr<RenderPassEncoder> beginRenderPass(const RenderPassEncoderDescriptor& descriptor) override;

    void copyBufferToBuffer(const BlitBuffer& src, const BlitBuffer& dst, uint64_t size) override;
    void copyBufferToTexture(const BlitTextureBuffer& buffer, const BlitTexture& texture, const Extent3D& extent) override;

    void copyTextureToBuffer() override;
    void copyTextureToTexture() override;

    CommandBuffer* end() override;
};

// Convert Helper
VkAttachmentLoadOp ToVkAttachmentLoadOp(LoadOp loadOp);
LoadOp ToVkAttachmentLoadOp(VkAttachmentLoadOp loadOp);
VkAttachmentStoreOp ToVkAttachmentStoreOp(StoreOp storeOp);
StoreOp ToStoreOp(VkAttachmentStoreOp storeOp);

} // namespace vkt