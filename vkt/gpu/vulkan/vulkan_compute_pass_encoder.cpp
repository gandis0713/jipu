#include "vulkan_compute_pass_encoder.h"
#include "vulkan_command_buffer.h"

namespace vkt
{

VulkanComputePassEncoder::VulkanComputePassEncoder(VulkanCommandBuffer* commandBuffer, const ComputePassEncoderDescriptor& descriptor)
    : ComputePassEncoder(commandBuffer, descriptor)
{
}

void VulkanComputePassEncoder::setPipeline(Pipeline* pipeline)
{
}

void VulkanComputePassEncoder::setBindingGroup(uint32_t index, BindingGroup* bindingGroup)
{
}

void VulkanComputePassEncoder::end()
{
}

} // namespace vkt