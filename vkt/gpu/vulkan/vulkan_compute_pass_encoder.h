#pragma once

#include "vkt/gpu/compute_pass_encoder.h"

namespace vkt
{

class VulkanCommandBuffer;
class VulkanComputePassEncoder : public ComputePassEncoder
{
public:
    VulkanComputePassEncoder() = delete;
    VulkanComputePassEncoder(VulkanCommandBuffer* commandBuffer, const ComputePassDescriptor& descriptor);
    ~VulkanComputePassEncoder() override = default;

public:
    void setPipeline(Pipeline* pipeline) override;
    void setBindingGroup(uint32_t index, BindingGroup* bindingGroup) override;
    void dispatch(uint32_t x, uint32_t y = 1, uint32_t z = 1) override;

    void end() override;
};

} // namespace vkt