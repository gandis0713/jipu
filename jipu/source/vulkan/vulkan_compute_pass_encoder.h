#pragma once

#include "jipu/compute_pass_encoder.h"
#include "vulkan_api.h"
#include "vulkan_export.h"

namespace jipu
{

class VulkanBindingGroup;
class VulkanComputePipeline;
class VulkanCommandBuffer;
class VULKAN_EXPORT VulkanComputePassEncoder : public ComputePassEncoder
{
public:
    VulkanComputePassEncoder() = delete;
    VulkanComputePassEncoder(VulkanCommandBuffer* commandBuffer, const ComputePassEncoderDescriptor& descriptor);
    ~VulkanComputePassEncoder() override = default;

public:
    void setPipeline(ComputePipeline& pipeline) override;
    void setBindingGroup(uint32_t index, BindingGroup* bindingGroup, std::vector<uint32_t> dynamicOffset = {}) override;
    void dispatch(uint32_t x, uint32_t y = 1, uint32_t z = 1) override;
    void end() override;

private:
    VulkanCommandBuffer* m_commandBuffer = nullptr;

private:
    VkPipeline m_pipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
};

} // namespace jipu