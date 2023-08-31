#pragma once

#include <cstdint>

namespace vkt
{

struct ComputePassEncoderDescriptor
{
};

class CommandBuffer;
class Pipeline;
class BindingGroup;
class ComputePassEncoder
{
public:
    ComputePassEncoder() = delete;
    ComputePassEncoder(CommandBuffer* commandBuffer, const ComputePassEncoderDescriptor& descriptor);
    virtual ~ComputePassEncoder() = default;

    virtual void setPipeline(Pipeline* pipeline) = 0;
    virtual void setBindingGroup(uint32_t index, BindingGroup* bindingGroup) = 0;

protected:
    CommandBuffer* m_commandBuffer = nullptr;
    ComputePassEncoderDescriptor m_descriptor{};
};

} // namespace vkt