#pragma once

#include "export.h"

#include "jipu/command_encoder.h"
#include "jipu/render_pass_encoder.h"
#include "utils/cast.h"
#include "vulkan_api.h"

namespace jipu
{

class VulkanCommandBuffer;
class JIPU_EXPERIMENTAL_EXPORT VulkanCommandEncoder : public CommandEncoder
{
public:
    VulkanCommandEncoder() = delete;
    VulkanCommandEncoder(VulkanCommandBuffer* commandBuffer, const CommandEncoderDescriptor& descriptor);
    ~VulkanCommandEncoder() override = default;

    std::unique_ptr<ComputePassEncoder> beginComputePass(const ComputePassDescriptor& descriptor) override;
    std::unique_ptr<RenderPassEncoder> beginRenderPass(const RenderPassDescriptor& descriptor) override;

    void copyBufferToBuffer(const BlitBuffer& src, const BlitBuffer& dst, uint64_t size) override;
    void copyBufferToTexture(const BlitTextureBuffer& buffer, const BlitTexture& texture, const Extent3D& extent) override;
    void copyTextureToBuffer(const BlitTexture& texture, const BlitTextureBuffer& buffer, const Extent3D& extent) override;
    void copyTextureToTexture(const BlitTexture& src, const BlitTexture& dst, const Extent3D& extent) override;

    CommandBuffer* finish() override;

public:
    std::unique_ptr<RenderPassEncoder> beginRenderPass(const std::vector<RenderPassDescriptor>& descriptors);

public:
    VulkanCommandBuffer* getCommandBuffer() const;

private:
    VulkanCommandBuffer* m_commandBuffer = nullptr;
};
DOWN_CAST(VulkanCommandEncoder, CommandEncoder);

} // namespace jipu