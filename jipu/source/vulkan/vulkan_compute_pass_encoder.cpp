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

VulkanComputePassEncoder::VulkanComputePassEncoder(VulkanCommandBuffer* commandBuffer, const ComputePassDescriptor& descriptor)
    : ComputePassEncoder(commandBuffer, descriptor)
{
    // do nothing.
}

void VulkanComputePassEncoder::setPipeline(Pipeline* pipeline)
{
    // TODO: receive ComputePipeline from parameter.
    m_pipeline = pipeline;

    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    VulkanComputePipeline* vulkanComputePipeline = downcast(static_cast<ComputePipeline*>(m_pipeline)); // TODO: not casting to ComputePipeline.
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());
    const VulkanAPI& vkAPI = downcast(vulkanDevice)->vkAPI;

    vkAPI.CmdBindPipeline(vulkanCommandBuffer->getVkCommandBuffer(),
                          VK_PIPELINE_BIND_POINT_COMPUTE,
                          vulkanComputePipeline->getVkPipeline());
}

void VulkanComputePassEncoder::setBindingGroup(uint32_t index, BindingGroup* bindingGroup, std::vector<uint32_t> dynamicOffset)
{
    m_bindingGroups[index] = bindingGroup;

    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = downcast(vulkanCommandBuffer->getDevice());
    auto vulkanPipelineLayout = downcast(m_pipeline->getPipelineLayout());
    auto vulkanBindingGroup = downcast(bindingGroup);

    const VulkanAPI& vkAPI = downcast(vulkanDevice)->vkAPI;

    VkDescriptorSet descriptorSet = vulkanBindingGroup->getVkDescriptorSet();

    vkAPI.CmdBindDescriptorSets(vulkanCommandBuffer->getVkCommandBuffer(),
                                VK_PIPELINE_BIND_POINT_COMPUTE,
                                vulkanPipelineLayout->getVkPipelineLayout(),
                                0,
                                1,
                                &descriptorSet,
                                static_cast<uint32_t>(dynamicOffset.size()),
                                dynamicOffset.data());
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
    auto vulkanCommandBuffer = downcast(m_commandBuffer);

    // TODO: generate stage from binding group.
    VkPipelineStageFlags flags = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
    vulkanCommandBuffer->setSignalPipelineStage(flags);
}

} // namespace jipu