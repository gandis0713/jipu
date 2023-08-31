#pragma once

#include "vkt/gpu/compute_pass_encoder.h"

namespace vkt
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
    void setBindingGroup(uint32_t index, BindingGroup* bindingGroup) override;

    void end() override;
};

} // namespace vkt