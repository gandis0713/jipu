#pragma once

#include "vkt/gpu/command_encoder.h"

namespace vkt
{

class VulkanCommandBuffer;
class VulkanCommandEncoder : public CommandEncoder
{
public:
    VulkanCommandEncoder() = delete;
    VulkanCommandEncoder(VulkanCommandBuffer* commandBuffer, const CommandEncoderDescriptor& descriptor);
    ~VulkanCommandEncoder() override = default;

    void startEncoding() override;
    void endEncoding() override;

    void setPipeline(Pipeline* pipeline) override;
    void setVertexBuffer(Buffer* buffer) override;
    void setIndexBuffer(Buffer* buffer) override;

    void draw(uint32_t vertexCount) override;
    void drawIndexed(uint32_t indexCount) override;
};
} // namespace vkt