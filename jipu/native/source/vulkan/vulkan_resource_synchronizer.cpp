#include "vulkan_resource_synchronizer.h"

#include "vulkan_binding_group.h"
#include "vulkan_binding_group_layout.h"
#include "vulkan_buffer.h"
#include "vulkan_command.h"
#include "vulkan_command_encoder.h"
#include "vulkan_texture.h"

namespace jipu
{
VulkanResourceSynchronizer::VulkanResourceSynchronizer(VulkanCommandEncoder* commandEncoder)
    : m_commandEncoder(commandEncoder)
{
}

void VulkanResourceSynchronizer::beginComputePass(BeginComputePassCommand* command)
{
    // do nothing.
}

void VulkanResourceSynchronizer::setComputePipeline(SetComputePipelineCommand* command)
{
    // do nothing.
}

void VulkanResourceSynchronizer::setComputeBindingGroup(SetBindGroupCommand* command)
{
    // consumer
    if (false) // TODO
    {
        PipelineBarrierCommand barrierCommand{
            { .type = CommandType::kPipelineBarrier },
            .srcStageMask = VK_PIPELINE_STAGE_NONE,
            .dstStageMask = VK_PIPELINE_STAGE_NONE,
            .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
        };

        auto bufferBindings = command->bindingGroup->getBufferBindings();
        for (auto& bufferBinding : bufferBindings)
        {
            if (findBufferProducer(bufferBinding.buffer))
            {
                auto producedBufferUsageInfo = extractBufferUsageInfo(bufferBinding.buffer);
                auto consumedBufferUsageInfo = BufferUsageInfo{ .stageFlags = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                                                .accessFlags = VK_ACCESS_SHADER_READ_BIT };

                barrierCommand.srcStageMask |= producedBufferUsageInfo.stageFlags;
                barrierCommand.dstStageMask |= consumedBufferUsageInfo.stageFlags;
                barrierCommand.bufferMemoryBarriers.push_back({
                    .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
                    .pNext = nullptr,
                    .srcAccessMask = producedBufferUsageInfo.accessFlags,
                    .dstAccessMask = consumedBufferUsageInfo.accessFlags,
                    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                    .buffer = downcast(bufferBinding.buffer)->getVkBuffer(),
                    .offset = 0,
                    .size = downcast(bufferBinding.buffer)->getSize(),
                });
            }
        }

        auto textureBindings = command->bindingGroup->getTextureBindings();
        for (auto& textureBinding : textureBindings)
        {
            if (findTextureProducer(textureBinding.textureView->getTexture()))
            {
                auto producedTextureUsageInfo = extractTextureUsageInfo(textureBinding.textureView->getTexture());
                auto consumedTextureUsageInfo = TextureUsageInfo{ .stageFlags = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                                                  .accessFlags = VK_ACCESS_SHADER_READ_BIT,
                                                                  .layout = VK_IMAGE_LAYOUT_GENERAL };

                barrierCommand.srcStageMask |= producedTextureUsageInfo.stageFlags;
                barrierCommand.dstStageMask |= consumedTextureUsageInfo.stageFlags;
                barrierCommand.imageMemoryBarriers.push_back({
                    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                    .pNext = nullptr,
                    .srcAccessMask = producedTextureUsageInfo.accessFlags,
                    .dstAccessMask = consumedTextureUsageInfo.accessFlags,
                    .oldLayout = producedTextureUsageInfo.layout,
                    .newLayout = consumedTextureUsageInfo.layout,
                    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                    .image = downcast(textureBinding.textureView->getTexture())->getVkImage(),
                    .subresourceRange = {
                        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                        .baseMipLevel = 0,
                        .levelCount = VK_REMAINING_MIP_LEVELS,
                        .baseArrayLayer = 0,
                        .layerCount = VK_REMAINING_ARRAY_LAYERS,
                    },
                });
            }
        }

        if (!barrierCommand.bufferMemoryBarriers.empty() || !barrierCommand.imageMemoryBarriers.empty())
            m_commandEncoder->getContext().commands.push_back(std::make_unique<PipelineBarrierCommand>(std::move(barrierCommand)));
    }

    // producer
    {
        auto bufferBindings = command->bindingGroup->getBufferBindings();
        for (auto& bufferBinding : bufferBindings)
        {
            m_ongoingPassResourceInfo.producer.buffers[bufferBinding.buffer] = BufferUsageInfo{
                .stageFlags = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                .accessFlags = VK_ACCESS_SHADER_WRITE_BIT,
            };
        }

        auto textureBindings = command->bindingGroup->getTextureBindings();
        for (auto& textureBinding : textureBindings)
        {
            m_ongoingPassResourceInfo.producer.textures[textureBinding.textureView->getTexture()] = TextureUsageInfo{
                .stageFlags = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                .accessFlags = VK_ACCESS_SHADER_WRITE_BIT,
                .layout = VK_IMAGE_LAYOUT_GENERAL,
            };
        }
    }
}

void VulkanResourceSynchronizer::dispatch(DispatchCommand* command)
{
    // do nothing.
}

void VulkanResourceSynchronizer::dispatchIndirect(DispatchIndirectCommand* command)
{
    // TODO
}

void VulkanResourceSynchronizer::endComputePass(EndComputePassCommand* command)
{
    m_passResourceInfos.push_back(std::move(m_ongoingPassResourceInfo));
    m_ongoingPassResourceInfo.clear();
}

void VulkanResourceSynchronizer::beginRenderPass(BeginRenderPassCommand* command)
{
    // do nothing.
}

void VulkanResourceSynchronizer::setRenderPipeline(SetRenderPipelineCommand* command)
{
    // do nothing.
}

void VulkanResourceSynchronizer::setVertexBuffer(SetVertexBufferCommand* command)
{
    // consumer
    {
        PipelineBarrierCommand barrierCommand{
            { .type = CommandType::kPipelineBarrier },
            .srcStageMask = VK_PIPELINE_STAGE_NONE,
            .dstStageMask = VK_PIPELINE_STAGE_NONE,
            .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
        };

        auto buffer = command->buffer;
        if (findBufferProducer(buffer))
        {
            auto producedBufferUsageInfo = extractBufferUsageInfo(buffer);
            auto consumedBufferUsageInfo = BufferUsageInfo{ .stageFlags = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
                                                            .accessFlags = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT };

            barrierCommand.srcStageMask |= producedBufferUsageInfo.stageFlags;
            barrierCommand.dstStageMask |= consumedBufferUsageInfo.stageFlags;
            barrierCommand.bufferMemoryBarriers.push_back({
                .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
                .pNext = nullptr,
                .srcAccessMask = producedBufferUsageInfo.accessFlags,
                .dstAccessMask = consumedBufferUsageInfo.accessFlags,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .buffer = downcast(buffer)->getVkBuffer(),
                .offset = 0,
                .size = downcast(buffer)->getSize(),
            });
        }

        if (!barrierCommand.bufferMemoryBarriers.empty())
            m_commandEncoder->getContext().commands.push_back(std::make_unique<PipelineBarrierCommand>(std::move(barrierCommand)));
    }
}

void VulkanResourceSynchronizer::setIndexBuffer(SetIndexBufferCommand* command)
{
    // consumer
    {
        PipelineBarrierCommand barrierCommand{
            { .type = CommandType::kPipelineBarrier },
            .srcStageMask = VK_PIPELINE_STAGE_NONE,
            .dstStageMask = VK_PIPELINE_STAGE_NONE,
            .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
        };

        auto buffer = command->buffer;
        if (findBufferProducer(buffer))
        {
            auto producedBufferUsageInfo = extractBufferUsageInfo(buffer);
            auto consumedBufferUsageInfo = BufferUsageInfo{ .stageFlags = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
                                                            .accessFlags = VK_ACCESS_INDEX_READ_BIT };

            barrierCommand.srcStageMask |= producedBufferUsageInfo.stageFlags;
            barrierCommand.dstStageMask |= consumedBufferUsageInfo.stageFlags;
            barrierCommand.bufferMemoryBarriers.push_back({
                .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
                .pNext = nullptr,
                .srcAccessMask = producedBufferUsageInfo.accessFlags,
                .dstAccessMask = consumedBufferUsageInfo.accessFlags,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .buffer = downcast(buffer)->getVkBuffer(),
                .offset = 0,
                .size = downcast(buffer)->getSize(),
            });
        }

        if (!barrierCommand.bufferMemoryBarriers.empty())
            m_commandEncoder->getContext().commands.push_back(std::make_unique<PipelineBarrierCommand>(std::move(barrierCommand)));
    }
}

void VulkanResourceSynchronizer::setViewport(SetViewportCommand* command)
{
    // do nothing.
}

void VulkanResourceSynchronizer::setScissor(SetScissorCommand* command)
{
    // do nothing.
}

void VulkanResourceSynchronizer::setBlendConstant(SetBlendConstantCommand* command)
{
    // do nothing.
}

void VulkanResourceSynchronizer::draw(DrawCommand* command)
{
    // do nothing.
}

void VulkanResourceSynchronizer::drawIndexed(DrawIndexedCommand* command)
{
    // do nothing.
}

void VulkanResourceSynchronizer::beginOcclusionQuery(BeginOcclusionQueryCommand* command)
{
    // do nothing.
}

void VulkanResourceSynchronizer::endOcclusionQuery(EndOcclusionQueryCommand* command)
{
    // do nothing.
}

void VulkanResourceSynchronizer::endRenderPass(EndRenderPassCommand* command)
{
    m_passResourceInfos.push_back(std::move(m_ongoingPassResourceInfo));
    m_ongoingPassResourceInfo.clear();
}

void VulkanResourceSynchronizer::setRenderBindingGroup(SetBindGroupCommand* command)
{
    // consumer
    {
        PipelineBarrierCommand barrierCommand{
            { .type = CommandType::kPipelineBarrier },
            .srcStageMask = VK_PIPELINE_STAGE_NONE,
            .dstStageMask = VK_PIPELINE_STAGE_NONE,
            .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
        };

        auto bindingGroup = command->bindingGroup;
        auto bindingGroupLayout = command->bindingGroup->getLayout();

        auto bufferBindings = bindingGroup->getBufferBindings();
        auto bufferBindingLayouts = bindingGroupLayout->getBufferBindingLayouts();
        for (auto i = 0; i < bufferBindings.size(); ++i)
        {
            auto& bufferBinding = bufferBindings[i];
            auto& bufferBindingLayout = bufferBindingLayouts[i];

            if (findBufferProducer(bufferBinding.buffer))
            {
                auto producedBufferUsageInfo = extractBufferUsageInfo(bufferBinding.buffer);
                auto consumedBufferUsageInfo = BufferUsageInfo{ .stageFlags = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
                                                                              VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, // TODO: set by shader stage.
                                                                .accessFlags = VK_ACCESS_SHADER_READ_BIT };

                if (bufferBindingLayout.stages & BindingStageFlagBits::kComputeStage)
                {
                    producedBufferUsageInfo.stageFlags |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
                }
                if (bufferBindingLayout.stages & BindingStageFlagBits::kVertexStage)
                {
                    producedBufferUsageInfo.stageFlags |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
                }
                if (bufferBindingLayout.stages & BindingStageFlagBits::kFragmentStage)
                {
                    producedBufferUsageInfo.stageFlags |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                }

                switch (bufferBindingLayout.type)
                {
                case BufferBindingType::kUniform:
                    producedBufferUsageInfo.accessFlags |= VK_ACCESS_UNIFORM_READ_BIT;
                    break;
                case BufferBindingType::kStorage:
                    producedBufferUsageInfo.accessFlags |= VK_ACCESS_SHADER_WRITE_BIT;
                    break;
                case BufferBindingType::kReadOnlyStorage:
                    producedBufferUsageInfo.accessFlags |= VK_ACCESS_SHADER_READ_BIT;
                    break;
                default:
                    producedBufferUsageInfo.accessFlags |= VK_ACCESS_SHADER_READ_BIT;
                    break;
                }

                barrierCommand.srcStageMask |= producedBufferUsageInfo.stageFlags;
                barrierCommand.dstStageMask |= consumedBufferUsageInfo.stageFlags;
                barrierCommand.bufferMemoryBarriers.push_back({
                    .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
                    .pNext = nullptr,
                    .srcAccessMask = producedBufferUsageInfo.accessFlags,
                    .dstAccessMask = consumedBufferUsageInfo.accessFlags,
                    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                    .buffer = downcast(bufferBinding.buffer)->getVkBuffer(),
                    .offset = 0,
                    .size = downcast(bufferBinding.buffer)->getSize(),
                });
            }
        }

        auto textureBindings = bindingGroup->getTextureBindings();
        auto textureBindingLayouts = bindingGroupLayout->getTextureBindingLayouts();
        for (auto i = 0; i < textureBindings.size(); ++i)
        {
            auto& textureBinding = textureBindings[i];
            auto& textureBindingLayout = textureBindingLayouts[i];

            if (findTextureProducer(textureBinding.textureView->getTexture()))
            {
                auto producedTextureUsageInfo = extractTextureUsageInfo(textureBinding.textureView->getTexture());
                auto consumedTextureUsageInfo = TextureUsageInfo{ .stageFlags = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
                                                                                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, // TODO: set by shader stage.
                                                                  .accessFlags = VK_ACCESS_SHADER_READ_BIT,
                                                                  .layout = VK_IMAGE_LAYOUT_GENERAL };

                if (textureBindingLayout.stages & BindingStageFlagBits::kComputeStage)
                {
                    producedTextureUsageInfo.stageFlags |= VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;
                }
                if (textureBindingLayout.stages & BindingStageFlagBits::kVertexStage)
                {
                    producedTextureUsageInfo.stageFlags |= VK_PIPELINE_STAGE_VERTEX_SHADER_BIT;
                }
                if (textureBindingLayout.stages & BindingStageFlagBits::kFragmentStage)
                {
                    producedTextureUsageInfo.stageFlags |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                }
                producedTextureUsageInfo.accessFlags |= VK_ACCESS_SHADER_READ_BIT;
                producedTextureUsageInfo.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

                barrierCommand.srcStageMask |= producedTextureUsageInfo.stageFlags;
                barrierCommand.dstStageMask |= consumedTextureUsageInfo.stageFlags;
                barrierCommand.imageMemoryBarriers.push_back({
                    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                    .pNext = nullptr,
                    .srcAccessMask = producedTextureUsageInfo.accessFlags,
                    .dstAccessMask = consumedTextureUsageInfo.accessFlags,
                    .oldLayout = producedTextureUsageInfo.layout,
                    .newLayout = consumedTextureUsageInfo.layout,
                    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                    .image = downcast(textureBinding.textureView->getTexture())->getVkImage(),
                    .subresourceRange = {
                        .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                        .baseMipLevel = 0,
                        .levelCount = VK_REMAINING_MIP_LEVELS,
                        .baseArrayLayer = 0,
                        .layerCount = VK_REMAINING_ARRAY_LAYERS,
                    },
                });
            }
        }

        if (!barrierCommand.bufferMemoryBarriers.empty() || !barrierCommand.imageMemoryBarriers.empty())
            m_commandEncoder->getContext().commands.push_back(std::make_unique<PipelineBarrierCommand>(std::move(barrierCommand)));
    }

    // producer
    {
        // TODO
    }
}

void VulkanResourceSynchronizer::copyBufferToBuffer(CopyBufferToBufferCommand* command)
{
    // do nothing.
}

void VulkanResourceSynchronizer::copyBufferToTexture(CopyBufferToTextureCommand* command)
{
    // do nothing.
}

void VulkanResourceSynchronizer::copyTextureToBuffer(CopyTextureToBufferCommand* command)
{
    // do nothing.
}

void VulkanResourceSynchronizer::copyTextureToTexture(CopyTextureToTextureCommand* command)
{
    // do nothing.
}

void VulkanResourceSynchronizer::resolveQuerySet(ResolveQuerySetCommand* command)
{
    // do nothing.
}

bool VulkanResourceSynchronizer::findBufferProducer(Buffer* buffer) const
{
    auto it = std::find_if(m_passResourceInfos.begin(), m_passResourceInfos.end(), [buffer](const PassResourceInfo& passResourceInfo) {
        return passResourceInfo.producer.buffers.find(buffer) != passResourceInfo.producer.buffers.end();
    });

    return it != m_passResourceInfos.end();
}

bool VulkanResourceSynchronizer::findTextureProducer(Texture* texture) const
{
    auto it = std::find_if(m_passResourceInfos.begin(), m_passResourceInfos.end(), [texture](const PassResourceInfo& passResourceInfo) {
        return passResourceInfo.producer.textures.find(texture) != passResourceInfo.producer.textures.end();
    });

    return it != m_passResourceInfos.end();
}

BufferUsageInfo VulkanResourceSynchronizer::extractBufferUsageInfo(Buffer* buffer)
{
    auto it = std::find_if(m_passResourceInfos.begin(), m_passResourceInfos.end(), [buffer](const PassResourceInfo& passResourceInfo) {
        return passResourceInfo.producer.buffers.find(buffer) != passResourceInfo.producer.buffers.end();
    });

    auto bufferUsageInfo = it->producer.buffers.at(buffer);
    it->producer.buffers.erase(buffer); // remove it

    return bufferUsageInfo;
}

TextureUsageInfo VulkanResourceSynchronizer::extractTextureUsageInfo(Texture* texture)
{
    auto it = std::find_if(m_passResourceInfos.begin(), m_passResourceInfos.end(), [texture](const PassResourceInfo& passResourceInfo) {
        return passResourceInfo.producer.textures.find(texture) != passResourceInfo.producer.textures.end();
    });

    auto textureUsageInfo = it->producer.textures.at(texture);
    it->producer.textures.erase(texture); // remove it

    return textureUsageInfo;
}

std::vector<PassResourceInfo> VulkanResourceSynchronizer::getPassResourceInfos() const
{
    return m_passResourceInfos;
}

} // namespace jipu