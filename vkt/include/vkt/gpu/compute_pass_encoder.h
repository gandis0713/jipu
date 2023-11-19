#pragma once

#include <cstdint>
#include <unordered_map>
#include <vector>

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
    virtual void setBindingGroup(uint32_t index, BindingGroup* bindingGroup, std::vector<uint32_t> dynamicOffset = {}) = 0;
    virtual void dispatch(uint32_t x, uint32_t y = 1, uint32_t z = 1) = 0;
    virtual void end() = 0;

protected:
    CommandBuffer* m_commandBuffer = nullptr;
    Pipeline* m_pipeline = nullptr;
    std::unordered_map<uint32_t, BindingGroup*> m_bindingGroups{};

    ComputePassEncoderDescriptor m_descriptor{};
};

} // namespace vkt