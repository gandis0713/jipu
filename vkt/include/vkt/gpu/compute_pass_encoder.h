#pragma once

#include <cstdint>
#include <unordered_map>

namespace vkt
{

struct ComputePassDescriptor
{
};

class CommandBuffer;
class Pipeline;
class BindingGroup;
class ComputePassEncoder
{
public:
    ComputePassEncoder() = delete;
    ComputePassEncoder(CommandBuffer* commandBuffer, const ComputePassDescriptor& descriptor);
    virtual ~ComputePassEncoder() = default;

    virtual void setPipeline(Pipeline* pipeline) = 0;
    virtual void setBindingGroup(uint32_t index, BindingGroup* bindingGroup) = 0;
    virtual void dispatch(uint32_t x, uint32_t y = 1, uint32_t z = 1) = 0;

    virtual void end() = 0;

protected:
    CommandBuffer* m_commandBuffer = nullptr;
    Pipeline* m_pipeline = nullptr;
    std::unordered_map<uint32_t, BindingGroup*> m_bindingGroups{};

    ComputePassDescriptor m_descriptor{};
};

} // namespace vkt