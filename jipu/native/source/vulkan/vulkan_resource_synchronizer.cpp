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
    sync();
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

    sync();
}

void VulkanResourceSynchronizer::setRenderPipeline(SetRenderPipelineCommand* command)
{
    // do nothing.
}

void VulkanResourceSynchronizer::setVertexBuffer(SetVertexBufferCommand* command)
{
    // do nothing.
}

void VulkanResourceSynchronizer::setIndexBuffer(SetIndexBufferCommand* command)
{
    // do nothing.
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
    // do nothing.
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

void VulkanResourceSynchronizer::cmdPipelineBarrier(const PipelineBarrier& barrier)
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

bool VulkanResourceSynchronizer::findSrcBuffer(Buffer* buffer) const
{
    auto& passResourceInfos = m_descriptor.passResourceInfos;

    auto begin = passResourceInfos.begin();
    auto end = passResourceInfos.begin() + currentPassIndex();
    auto it = std::find_if(begin, end, [buffer](const PassResourceInfo& passResourceInfo) {
        return passResourceInfo.src.buffers.find(buffer) != passResourceInfo.src.buffers.end();
    });

    return it != passResourceInfos.end();
}

bool VulkanResourceSynchronizer::findSrcTexture(Texture* texture) const
{
    auto& passResourceInfos = m_descriptor.passResourceInfos;

    auto begin = passResourceInfos.begin();
    auto end = passResourceInfos.begin() + currentPassIndex();
    auto it = std::find_if(begin, end, [texture](const PassResourceInfo& passResourceInfo) {
        return passResourceInfo.src.textures.find(texture) != passResourceInfo.src.textures.end();
    });

    return it != passResourceInfos.end();
}

BufferUsageInfo VulkanResourceSynchronizer::extractSrcBufferUsageInfo(Buffer* buffer)
{
    auto& passResourceInfos = m_descriptor.passResourceInfos;

    auto begin = passResourceInfos.begin();
    auto end = passResourceInfos.begin() + currentPassIndex();
    auto it = std::find_if(begin, end, [buffer](const PassResourceInfo& passResourceInfo) {
        return passResourceInfo.src.buffers.find(buffer) != passResourceInfo.src.buffers.end();
    });

    auto bufferUsageInfo = it->src.buffers.at(buffer);
    it->src.buffers.erase(buffer); // remove it

    return bufferUsageInfo;
}

TextureUsageInfo VulkanResourceSynchronizer::extractSrcTextureUsageInfo(Texture* texture)
{
    auto& passResourceInfos = m_descriptor.passResourceInfos;

    auto begin = passResourceInfos.begin();
    auto end = passResourceInfos.begin() + currentPassIndex();
    auto it = std::find_if(begin, end, [texture](const PassResourceInfo& passResourceInfo) {
        return passResourceInfo.src.textures.find(texture) != passResourceInfo.src.textures.end();
    });

    auto textureUsageInfo = it->src.textures.at(texture);
    it->src.textures.erase(texture); // remove it

    return textureUsageInfo;
}

const PassResourceInfo& VulkanResourceSynchronizer::getCurrentPassResourceInfo() const
{
    return m_descriptor.passResourceInfos[currentPassIndex()];
}

void VulkanResourceSynchronizer::increasePassIndex()
{
    ++m_currentPassIndex;
}

int32_t VulkanResourceSynchronizer::currentPassIndex() const
{
    return m_currentPassIndex;
}

void VulkanResourceSynchronizer::sync()
{
    PipelineBarrier pipelineBarrier{
        .srcStageMask = VK_PIPELINE_STAGE_NONE,
        .dstStageMask = VK_PIPELINE_STAGE_NONE,
        .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
    };

    const auto& currentPassResourceInfo = getCurrentPassResourceInfo();
    for (const auto& [buffer, dstBufferUsageInfo] : currentPassResourceInfo.src.buffers)
    {
        if (findSrcBuffer(buffer))
        {
            auto srcBufferUsageInfo = extractSrcBufferUsageInfo(buffer);

            pipelineBarrier.srcStageMask |= srcBufferUsageInfo.stageFlags;
            pipelineBarrier.dstStageMask |= dstBufferUsageInfo.stageFlags;
            pipelineBarrier.bufferMemoryBarriers.push_back({
                .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
                .pNext = nullptr,
                .srcAccessMask = srcBufferUsageInfo.accessFlags,
                .dstAccessMask = dstBufferUsageInfo.accessFlags,
                .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
                .buffer = downcast(buffer)->getVkBuffer(),
                .offset = 0,
                .size = downcast(buffer)->getSize(),
            });
        }
    }

    for (const auto& [texture, dstTextureUsageInfo] : currentPassResourceInfo.src.textures)
    {
        if (findSrcTexture(texture))
        {
            auto srcTextureUsageInfo = extractSrcTextureUsageInfo(texture);

            pipelineBarrier.srcStageMask |= srcTextureUsageInfo.stageFlags;
            pipelineBarrier.dstStageMask |= dstTextureUsageInfo.stageFlags;
            pipelineBarrier.imageMemoryBarriers.push_back({
                .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
                .pNext = nullptr,
                .srcAccessMask = srcTextureUsageInfo.accessFlags,
                .dstAccessMask = dstTextureUsageInfo.accessFlags,
                .oldLayout = srcTextureUsageInfo.layout,
                .newLayout = dstTextureUsageInfo.layout,
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
        cmdPipelineBarrier(pipelineBarrier);
    }
}

} // namespace jipu