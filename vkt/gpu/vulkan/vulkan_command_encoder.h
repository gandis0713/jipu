#pragma once

#include "vkt/gpu/command_encoder.h"
#include "vulkan_api.h"

namespace vkt
{

class VulkanCommandBuffer;
class VulkanRenderCommandEncoder : public RenderCommandEncoder
{
public:
    VulkanRenderCommandEncoder() = delete;
    VulkanRenderCommandEncoder(VulkanCommandBuffer* commandBuffer, const RenderCommandEncoderDescriptor& descriptor);
    ~VulkanRenderCommandEncoder() override = default;

    void begin() override;
    void end() override;

    void setPipeline(Pipeline* pipeline) override;
    void setVertexBuffer(Buffer* buffer) override;
    void setIndexBuffer(Buffer* buffer) override;

    void draw(uint32_t vertexCount) override;
    void drawIndexed(uint32_t indexCount) override;
};

// Convert Helper
VkAttachmentLoadOp ToVkAttachmentLoadOp(LoadOp loadOp);
LoadOp ToVkAttachmentLoadOp(VkAttachmentLoadOp loadOp);
VkAttachmentStoreOp ToVkAttachmentStoreOp(StoreOp storeOp);
StoreOp ToStoreOp(VkAttachmentStoreOp storeOp);

} // namespace vkt