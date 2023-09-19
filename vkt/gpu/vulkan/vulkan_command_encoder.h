#pragma once

#include "vkt/gpu/command_encoder.h"
#include "vkt/gpu/render_pass_encoder.h"
#include "vulkan_api.h"

namespace vkt
{

class VulkanCommandBuffer;
class VulkanCommandEncoder : public CommandEncoder
{
public:
    VulkanCommandEncoder() = delete;
    VulkanCommandEncoder(VulkanCommandBuffer* commandBuffer, const CommandEncoderDescriptor& descriptor);
    ~VulkanCommandEncoder() override = default;

    std::unique_ptr<ComputePassEncoder> beginComputePass(const ComputePassDescriptor& descriptor) override;
    std::unique_ptr<RenderPassEncoder> beginRenderPass(const RenderPassDescriptor& descriptor) override;

    void copyBufferToBuffer(const BlitBuffer& src, const BlitBuffer& dst, uint64_t size) override;
    void copyBufferToTexture(const BlitTextureBuffer& buffer, const BlitTexture& texture, const Extent3D& extent) override;

    void copyTextureToBuffer() override;
    void copyTextureToTexture() override;

    CommandBuffer* finish() override;
};

} // namespace vkt