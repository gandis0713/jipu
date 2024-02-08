#pragma once

#include "export.h"

#include "jipu/render_pass_encoder.h"
#include "vulkan_api.h"
#include "vulkan_render_pass.h"

namespace jipu
{

class VulkanCommandBuffer;
class JIPU_EXPERIMENTAL_EXPORT VulkanRenderPassEncoder : public RenderPassEncoder
{
public:
    VulkanRenderPassEncoder() = delete;
    VulkanRenderPassEncoder(VulkanCommandBuffer* commandBuffer, const RenderPassDescriptor& descriptor);
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
VulkanRenderPassDescriptor generateVulkanRenderPassDescriptor(const RenderPassEncoderDescriptor& descriptor);

// Convert Helper
VkIndexType ToVkIndexType(IndexFormat format);

} // namespace jipu