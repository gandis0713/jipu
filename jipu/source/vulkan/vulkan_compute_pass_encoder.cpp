#include "vulkan_compute_pass_encoder.h"
#include "vulkan_binding_group.h"
#include "vulkan_buffer.h"
#include "vulkan_command_buffer.h"
#include "vulkan_device.h"
#include "vulkan_pipeline.h"
#include "vulkan_pipeline_layout.h"

#include "vulkan_api.h"

namespace jipu
{

VulkanComputePassEncoder::VulkanComputePassEncoder(VulkanCommandBuffer& commandBuffer, const ComputePassEncoderDescriptor& descriptor)
    : m_commandBuffer(commandBuffer)
{
    // do nothing.
}

void VulkanComputePassEncoder::setPipeline(ComputePipeline& pipeline)
{
    auto& vulkanComputePipeline = static_cast<VulkanComputePipeline&>(pipeline);

    m_pipeline = vulkanComputePipeline.getVkPipeline();
    m_pipelineLayout = downcast(vulkanComputePipeline.getPipelineLayout())->getVkPipelineLayout();

    auto& vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer.getDevice());
    const VulkanAPI& vkAPI = downcast(vulkanDevice)->vkAPI;

    vkAPI.CmdBindPipeline(vulkanCommandBuffer.getVkCommandBuffer(),
                          VK_PIPELINE_BIND_POINT_COMPUTE,
                          m_pipeline);
}

void VulkanComputePassEncoder::setBindingGroup(uint32_t index, BindingGroup* bindingGroup, std::vector<uint32_t> dynamicOffset)
{
    auto& vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer.getDevice());
    auto vulkanBindingGroup = downcast(bindingGroup);

    const VulkanAPI& vkAPI = downcast(vulkanDevice)->vkAPI;

    VkDescriptorSet descriptorSet = vulkanBindingGroup->getVkDescriptorSet();

    vkAPI.CmdBindDescriptorSets(vulkanCommandBuffer.getVkCommandBuffer(),
                                VK_PIPELINE_BIND_POINT_COMPUTE,
                                m_pipelineLayout,
                                0,
                                1,
                                &descriptorSet,
                                static_cast<uint32_t>(dynamicOffset.size()),
                                dynamicOffset.data());
}

void VulkanComputePassEncoder::dispatch(uint32_t x, uint32_t y, uint32_t z)
{
    auto& vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer.getDevice());
    const VulkanAPI& vkAPI = downcast(vulkanDevice)->vkAPI;

    vkAPI.CmdDispatch(vulkanCommandBuffer.getVkCommandBuffer(), x, y, z);
}

void VulkanComputePassEncoder::end()
{
    auto& vulkanCommandBuffer = downcast(m_commandBuffer);

    // TODO: generate stage from binding group.
    VkPipelineStageFlags flags = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
    vulkanCommandBuffer.setSignalPipelineStage(flags);
}

} // namespace jipu