#include "vulkan_resource_synchronizer.h"

#include "vulkan_binding_group.h"
#include "vulkan_binding_group_layout.h"
#include "vulkan_buffer.h"
#include "vulkan_command.h"
#include "vulkan_command_buffer.h"
#include "vulkan_command_encoder.h"
#include "vulkan_device.h"
#include "vulkan_texture.h"

namespace jipu
{
VulkanResourceSynchronizer::VulkanResourceSynchronizer(VulkanCommandBuffer* commandBuffer, const VulkanResourceSynchronizerDescriptor& descriptor)
    : m_commandBuffer(commandBuffer)
    , m_descriptor(descriptor)
    , m_currentPassIndex(-1)
{
}

void VulkanResourceSynchronizer::beginComputePass(BeginComputePassCommand* command)
{
    increasePassIndex();
}

void VulkanResourceSynchronizer::setComputePipeline(SetComputePipelineCommand* command)
{
    // do nothing.
}

void VulkanResourceSynchronizer::setComputeBindingGroup(SetBindGroupCommand* command)
{
    // do nothing.
}

void VulkanResourceSynchronizer::dispatch(DispatchCommand* command)
{
    // consumer
    {
        PipelineBarrier pipelineBarrier{
            .srcStageMask = VK_PIPELINE_STAGE_NONE,
            .dstStageMask = VK_PIPELINE_STAGE_NONE,
            .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
        };

        auto& currentPassResourceInfo = m_descriptor.passResourceInfos[currentPassIndex()];
        for (const auto& [buffer, bufferUsageInfo] : currentPassResourceInfo.consumer.buffers)
        {
            if (findProducedBuffer(buffer))
            {
                auto producedBufferUsageInfo = extractProducedBufferUsageInfo(buffer);

                pipelineBarrier.srcStageMask |= producedBufferUsageInfo.stageFlags;
                pipelineBarrier.dstStageMask |= bufferUsageInfo.stageFlags;
                pipelineBarrier.bufferMemoryBarriers.push_back({
                    .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
                    .pNext = nullptr,
                    .srcAccessMask = producedBufferUsageInfo.accessFlags,
                    .dstAccessMask = bufferUsageInfo.accessFlags,
                    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                    .buffer = downcast(buffer)->getVkBuffer(),
                    .offset = 0,
                    .size = downcast(buffer)->getSize(),
                });
            }
        }

        for (const auto& [texture, textureUsageInfo] : currentPassResourceInfo.consumer.textures)
        {
            if (findProducedTexture(texture))
            {
                auto producedTextureUsageInfo = extractProducedTextureUsageInfo(texture);
                auto consumedTextureUsageInfo = TextureUsageInfo{ .stageFlags = VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                                                                  .accessFlags = VK_ACCESS_SHADER_READ_BIT,
                                                                  .layout = VK_IMAGE_LAYOUT_GENERAL };

                pipelineBarrier.srcStageMask |= producedTextureUsageInfo.stageFlags;
                pipelineBarrier.dstStageMask |= consumedTextureUsageInfo.stageFlags;
                pipelineBarrier.imageMemoryBarriers.push_back({
                    .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                    .pNext = nullptr,
                    .srcAccessMask = producedTextureUsageInfo.accessFlags,
                    .dstAccessMask = consumedTextureUsageInfo.accessFlags,
                    .oldLayout = producedTextureUsageInfo.layout,
                    .newLayout = consumedTextureUsageInfo.layout,
                    .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                    .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                    .image = downcast(texture)->getVkImage(),
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

        if (!pipelineBarrier.bufferMemoryBarriers.empty() || !pipelineBarrier.imageMemoryBarriers.empty())
        {
            sync(pipelineBarrier);
        }
    }
}

void VulkanResourceSynchronizer::dispatchIndirect(DispatchIndirectCommand* command)
{
    // TODO
}

void VulkanResourceSynchronizer::endComputePass(EndComputePassCommand* command)
{
    // do nothing.
}

void VulkanResourceSynchronizer::beginRenderPass(BeginRenderPassCommand* command)
{
    increasePassIndex();
}

void VulkanResourceSynchronizer::setRenderPipeline(SetRenderPipelineCommand* command)
{
    // do nothing.
}

void VulkanResourceSynchronizer::setVertexBuffer(SetVertexBufferCommand* command)
{
    // consumer
    {
        PipelineBarrier pipelineBarrier{
            .srcStageMask = VK_PIPELINE_STAGE_NONE,
            .dstStageMask = VK_PIPELINE_STAGE_NONE,
            .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
        };

        auto buffer = command->buffer;
        if (findProducedBuffer(buffer))
        {
            auto producedBufferUsageInfo = extractProducedBufferUsageInfo(buffer);
            auto consumedBufferUsageInfo = BufferUsageInfo{ .stageFlags = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
                                                            .accessFlags = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT };

            pipelineBarrier.srcStageMask |= producedBufferUsageInfo.stageFlags;
            pipelineBarrier.dstStageMask |= consumedBufferUsageInfo.stageFlags;
            pipelineBarrier.bufferMemoryBarriers.push_back({
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

        if (!pipelineBarrier.bufferMemoryBarriers.empty())
        {
            sync(pipelineBarrier);
        }
    }
}

void VulkanResourceSynchronizer::setIndexBuffer(SetIndexBufferCommand* command)
{
    // consumer
    {
        PipelineBarrier pipelineBarrier{
            .srcStageMask = VK_PIPELINE_STAGE_NONE,
            .dstStageMask = VK_PIPELINE_STAGE_NONE,
            .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
        };

        auto buffer = command->buffer;
        if (findProducedBuffer(buffer))
        {
            auto producedBufferUsageInfo = extractProducedBufferUsageInfo(buffer);
            auto consumedBufferUsageInfo = BufferUsageInfo{ .stageFlags = VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
                                                            .accessFlags = VK_ACCESS_INDEX_READ_BIT };

            pipelineBarrier.srcStageMask |= producedBufferUsageInfo.stageFlags;
            pipelineBarrier.dstStageMask |= consumedBufferUsageInfo.stageFlags;
            pipelineBarrier.bufferMemoryBarriers.push_back({
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

        if (!pipelineBarrier.bufferMemoryBarriers.empty())
        {
            sync(pipelineBarrier);
        }
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
    // do nothing.
}

void VulkanResourceSynchronizer::setRenderBindingGroup(SetBindGroupCommand* command)
{
    // consumer
    {
        PipelineBarrier pipelineBarrier{
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

            if (findProducedBuffer(bufferBinding.buffer))
            {
                auto producedBufferUsageInfo = extractProducedBufferUsageInfo(bufferBinding.buffer);
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

                pipelineBarrier.srcStageMask |= producedBufferUsageInfo.stageFlags;
                pipelineBarrier.dstStageMask |= consumedBufferUsageInfo.stageFlags;
                pipelineBarrier.bufferMemoryBarriers.push_back({
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

            if (findProducedTexture(textureBinding.textureView->getTexture()))
            {
                auto producedTextureUsageInfo = extractProducedTextureUsageInfo(textureBinding.textureView->getTexture());
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

                pipelineBarrier.srcStageMask |= producedTextureUsageInfo.stageFlags;
                pipelineBarrier.dstStageMask |= consumedTextureUsageInfo.stageFlags;
                pipelineBarrier.imageMemoryBarriers.push_back({
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

        if (!pipelineBarrier.bufferMemoryBarriers.empty() || !pipelineBarrier.imageMemoryBarriers.empty())
        {
            sync(pipelineBarrier);
        }
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

void VulkanResourceSynchronizer::sync(const PipelineBarrier& barrier)
{
    auto& srcStageMask = barrier.srcStageMask;
    auto& dstStageMask = barrier.dstStageMask;
    auto& dependencyFlags = barrier.dependencyFlags;
    auto& memoryBarriers = barrier.memoryBarriers;
    auto& bufferMemoryBarriers = barrier.bufferMemoryBarriers;
    auto& imageMemoryBarriers = barrier.imageMemoryBarriers;

    auto vulkanCommandBuffer = downcast(m_commandBuffer);
    auto vulkanDevice = vulkanCommandBuffer->getDevice();
    const VulkanAPI& vkAPI = vulkanDevice->vkAPI;

    vkAPI.CmdPipelineBarrier(vulkanCommandBuffer->getVkCommandBuffer(),
                             srcStageMask,
                             dstStageMask,
                             dependencyFlags,
                             static_cast<uint32_t>(memoryBarriers.size()),
                             memoryBarriers.data(),
                             static_cast<uint32_t>(memoryBarriers.size()),
                             bufferMemoryBarriers.data(),
                             static_cast<uint32_t>(memoryBarriers.size()),
                             imageMemoryBarriers.data());
}

bool VulkanResourceSynchronizer::findProducedBuffer(Buffer* buffer) const
{
    auto& passResourceInfos = m_descriptor.passResourceInfos;

    auto begin = passResourceInfos.begin();
    auto end = passResourceInfos.begin() + currentPassIndex();
    auto it = std::find_if(begin, end, [buffer](const PassResourceInfo& passResourceInfo) {
        return passResourceInfo.producer.buffers.find(buffer) != passResourceInfo.producer.buffers.end();
    });

    return it != passResourceInfos.end();
}

bool VulkanResourceSynchronizer::findProducedTexture(Texture* texture) const
{
    auto& passResourceInfos = m_descriptor.passResourceInfos;

    auto begin = passResourceInfos.begin();
    auto end = passResourceInfos.begin() + currentPassIndex();
    auto it = std::find_if(begin, end, [texture](const PassResourceInfo& passResourceInfo) {
        return passResourceInfo.producer.textures.find(texture) != passResourceInfo.producer.textures.end();
    });

    return it != passResourceInfos.end();
}

BufferUsageInfo VulkanResourceSynchronizer::extractProducedBufferUsageInfo(Buffer* buffer)
{
    auto& passResourceInfos = m_descriptor.passResourceInfos;

    auto begin = passResourceInfos.begin();
    auto end = passResourceInfos.begin() + currentPassIndex();
    auto it = std::find_if(begin, end, [buffer](const PassResourceInfo& passResourceInfo) {
        return passResourceInfo.producer.buffers.find(buffer) != passResourceInfo.producer.buffers.end();
    });

    auto bufferUsageInfo = it->producer.buffers.at(buffer);
    it->producer.buffers.erase(buffer); // remove it

    return bufferUsageInfo;
}

TextureUsageInfo VulkanResourceSynchronizer::extractProducedTextureUsageInfo(Texture* texture)
{
    auto& passResourceInfos = m_descriptor.passResourceInfos;

    auto begin = passResourceInfos.begin();
    auto end = passResourceInfos.begin() + currentPassIndex();
    auto it = std::find_if(begin, end, [texture](const PassResourceInfo& passResourceInfo) {
        return passResourceInfo.producer.textures.find(texture) != passResourceInfo.producer.textures.end();
    });

    auto textureUsageInfo = it->producer.textures.at(texture);
    it->producer.textures.erase(texture); // remove it

    return textureUsageInfo;
}

void VulkanResourceSynchronizer::increasePassIndex()
{
    ++m_currentPassIndex;
}

int32_t VulkanResourceSynchronizer::currentPassIndex() const
{
    return m_currentPassIndex;
}

} // namespace jipu