#include "vulkan_compute_pass_encoder.h"
#include "vulkan_command_buffer.h"
#include "vulkan_device.h"
#include "vulkan_pipeline.h"

#include "vulkan_api.h"

namespace vkt
{

VulkanComputePassEncoder::VulkanComputePassEncoder(VulkanCommandBuffer* commandBuffer, const ComputePassEncoderDescriptor& descriptor)
    : ComputePassEncoder(commandBuffer, descriptor)
{
    // do nothing.
}

void VulkanComputePassEncoder::setPipeline(Pipeline* pipeline)
{
    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    VulkanComputePipeline* vulkanComputePipeline = downcast(static_cast<ComputePipeline*>(pipeline)); // TODO: downcasting to RenderPipeline.
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());
    const VulkanAPI& vkAPI = downcast(vulkanDevice)->vkAPI;

    // TODO: set pipeline.
}

void VulkanComputePassEncoder::setBindingGroup(uint32_t index, BindingGroup* bindingGroup)
{
    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());
    const VulkanAPI& vkAPI = downcast(vulkanDevice)->vkAPI;

    // TODO: set binding group.
}

void VulkanComputePassEncoder::dispatch(uint32_t x, uint32_t y, uint32_t z)
{
    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());
    const VulkanAPI& vkAPI = downcast(vulkanDevice)->vkAPI;

    vkAPI.CmdDispatch(vulkanCommandBuffer->getVkCommandBuffer(), x, y, z);
}

void VulkanComputePassEncoder::end()
{
}

} // namespace vkt