#include "vulkan_inflight_context.h"

#include "vulkan_binding_group.h"
#include "vulkan_binding_group_layout.h"
#include "vulkan_buffer.h"
#include "vulkan_command_buffer.h"
#include "vulkan_framebuffer.h"
#include "vulkan_pipeline.h"
#include "vulkan_pipeline_layout.h"
#include "vulkan_render_pass.h"
#include "vulkan_sampler.h"
#include "vulkan_texture.h"
#include "vulkan_texture_view.h"

namespace jipu
{

InflightResource VulkanInflightContext::generate(std::vector<CommandBuffer*> commandBuffers)
{
    InflightResource inflightResource{};
    for (auto& commandBuffer : commandBuffers)
    {
        auto vulkanCommandBuffer = downcast(commandBuffer);
        auto& commands = vulkanCommandBuffer->getCommandEncodingResult().commands;
        for (auto& command : commands)
        {
            switch (command->type)
            {
            case CommandType::kBeginComputePass:
            case CommandType::kEndComputePass:
            case CommandType::kDispatch:
            case CommandType::kDispatchIndirect:
            default:
                break;
            case CommandType::kSetComputePipeline: {
                auto cmd = reinterpret_cast<SetComputePipelineCommand*>(command.get());
                inflightResource.pipelines.insert(downcast(cmd->pipeline)->getVkPipeline());
                inflightResource.pipelineLayouts.insert(downcast(cmd->pipeline->getPipelineLayout())->getVkPipelineLayout());
            }
            break;
            case CommandType::kSetComputeBindGroup: {
                auto cmd = reinterpret_cast<SetBindGroupCommand*>(command.get());
                inflightResource.descriptorSetLayouts.insert(downcast(cmd->bindingGroup->getLayout())->getVkDescriptorSetLayout());
                for (auto& binding : cmd->bindingGroup->getBufferBindings())
                {
                    inflightResource.buffers.insert(downcast(binding.buffer)->getVkBuffer());
                }
                for (auto& binding : cmd->bindingGroup->getSmaplerBindings())
                {
                    inflightResource.samplers.insert(downcast(binding.sampler)->getVkSampler());
                }
                for (auto& binding : cmd->bindingGroup->getTextureBindings())
                {
                    inflightResource.images.insert(downcast(binding.textureView->getTexture())->getVkImage());
                    inflightResource.imageViews.insert(downcast(binding.textureView)->getVkImageView());
                }
            }
            break;
            case CommandType::kBeginRenderPass: {
                auto cmd = reinterpret_cast<BeginRenderPassCommand*>(command.get());
                for (auto& colorAttachment : cmd->framebuffer->getColorAttachments())
                {
                    inflightResource.images.insert(downcast(colorAttachment.renderView->getTexture())->getVkImage());
                    inflightResource.imageViews.insert(downcast(colorAttachment.renderView)->getVkImageView());
                    if (colorAttachment.resolveView)
                    {
                        inflightResource.images.insert(downcast(colorAttachment.resolveView->getTexture())->getVkImage());
                        inflightResource.imageViews.insert(downcast(colorAttachment.resolveView)->getVkImageView());
                    }
                }
                inflightResource.renderPasses.insert(cmd->renderPass->getVkRenderPass());
            }
            break;
            case CommandType::kSetRenderPipeline: {
                auto cmd = reinterpret_cast<SetRenderPipelineCommand*>(command.get());
                inflightResource.pipelines.insert(downcast(cmd->pipeline)->getVkPipeline());
                inflightResource.pipelineLayouts.insert(downcast(cmd->pipeline->getPipelineLayout())->getVkPipelineLayout());
            }
            break;
            case CommandType::kSetRenderBindGroup: {
                auto cmd = reinterpret_cast<SetBindGroupCommand*>(command.get());
                inflightResource.descriptorSetLayouts.insert(downcast(cmd->bindingGroup->getLayout())->getVkDescriptorSetLayout());
                for (auto& binding : cmd->bindingGroup->getBufferBindings())
                {
                    inflightResource.buffers.insert(downcast(binding.buffer)->getVkBuffer());
                }
                for (auto& binding : cmd->bindingGroup->getSmaplerBindings())
                {
                    inflightResource.samplers.insert(downcast(binding.sampler)->getVkSampler());
                }
                for (auto& binding : cmd->bindingGroup->getTextureBindings())
                {
                    inflightResource.images.insert(downcast(binding.textureView->getTexture())->getVkImage());
                    inflightResource.imageViews.insert(downcast(binding.textureView)->getVkImageView());
                }
            }
            break;
            case CommandType::kSetIndexBuffer: {
                auto cmd = reinterpret_cast<SetIndexBufferCommand*>(command.get());
                inflightResource.buffers.insert(downcast(cmd->buffer)->getVkBuffer());
            }
            break;
            case CommandType::kSetVertexBuffer: {
                auto cmd = reinterpret_cast<SetVertexBufferCommand*>(command.get());
                inflightResource.buffers.insert(downcast(cmd->buffer)->getVkBuffer());
            }
            break;
            }
        }
    }

    return inflightResource;
}

VulkanInflightContext::VulkanInflightContext(VulkanDevice* device)
    : m_device(device)
{
}

VulkanInflightContext::~VulkanInflightContext()
{
}

VkFence VulkanInflightContext::add(VkQueue queue, InflightResource resource)
{
    return VK_NULL_HANDLE;
}

void VulkanInflightContext::clear(VkQueue queue)
{
}

void VulkanInflightContext::clearAll()
{
}

} // namespace jipu