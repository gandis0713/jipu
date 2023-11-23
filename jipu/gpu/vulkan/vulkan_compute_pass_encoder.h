#pragma once

#include "jipu/gpu/compute_pass_encoder.h"

namespace jipu
{

class VulkanCommandBuffer;
class VulkanComputePassEncoder : public ComputePassEncoder
{
public:
    VulkanComputePassEncoder() = delete;
    VulkanComputePassEncoder(VulkanCommandBuffer* commandBuffer, const ComputePassEncoderDescriptor& descriptor);
    ~VulkanComputePassEncoder() override = default;

public:
    void setPipeline(Pipeline* pipeline) override;
    void setBindingGroup(uint32_t index, BindingGroup* bindingGroup, std::vector<uint32_t> dynamicOffset = {}) override;
    void dispatch(uint32_t x, uint32_t y = 1, uint32_t z = 1) override;
    void end() override;
};

} // namespace jipu