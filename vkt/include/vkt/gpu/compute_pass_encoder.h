#pragma once

namespace vkt
{

struct ComputePassEncoderDescriptor
{
};

class CommandBuffer;
class ComputePassEncoder
{
public:
    ComputePassEncoder() = delete;
    ComputePassEncoder(CommandBuffer* commandBuffer, const ComputePassEncoderDescriptor& descriptor);
    virtual ~ComputePassEncoder() = default;

protected:
    CommandBuffer* m_commandBuffer = nullptr;
    ComputePassEncoderDescriptor m_descriptor{};
};

} // namespace vkt