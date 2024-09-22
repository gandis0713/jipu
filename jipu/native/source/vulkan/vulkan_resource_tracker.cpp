#include "vulkan_resource_tracker.h"

#include "vulkan_buffer.h"
#include "vulkan_texture.h"

namespace jipu
{

void VulkanResourceTracker::reset()
{
    m_passResourceInfos.clear();
}

void VulkanResourceTracker::beginComputePass(BeginComputePassCommand* command)
{
    // do nothing.
}

void VulkanResourceTracker::setComputePipeline(SetComputePipelineCommand* command)
{
    // do nothing.
}

void VulkanResourceTracker::setComputeBindingGroup(SetBindGroupCommand* command)
{
    // write
    {
        auto bufferBindings = command->bindingGroup->getBufferBindings();
        for (auto& bufferBinding : bufferBindings)
        {
            m_ongoingPassResourceInfo.write.buffers[bufferBinding.buffer] = {
                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                VK_ACCESS_SHADER_WRITE_BIT,
            };
        }

        auto textureBindings = command->bindingGroup->getTextureBindings();
        for (auto& textureBinding : textureBindings)
        {
            m_ongoingPassResourceInfo.write.textures[textureBinding.textureView->getTexture()] = {
                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                VK_ACCESS_SHADER_WRITE_BIT,
                VK_IMAGE_LAYOUT_GENERAL,
            };
        }
    }

    // read
    if (false)
    {
        auto bufferBindings = command->bindingGroup->getBufferBindings();
        for (auto& bufferBinding : bufferBindings)
        {
            m_ongoingPassResourceInfo.read.buffers[bufferBinding.buffer] = {
                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                VK_ACCESS_SHADER_READ_BIT,
            };
        }

        auto textureBindings = command->bindingGroup->getTextureBindings();
        for (auto& textureBinding : textureBindings)
        {
            m_ongoingPassResourceInfo.read.textures[textureBinding.textureView->getTexture()] = {
                VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
                VK_ACCESS_SHADER_READ_BIT,
                VK_IMAGE_LAYOUT_GENERAL,
            };
        }
    }
}

void VulkanResourceTracker::dispatch(DispatchCommand* command)
{
    // do nothing.
}

void VulkanResourceTracker::dispatchIndirect(DispatchIndirectCommand* command)
{
    // TODO
}

void VulkanResourceTracker::endComputePass(EndComputePassCommand* command)
{
    m_passResourceInfos.push_back(std::move(m_ongoingPassResourceInfo));
    m_ongoingPassResourceInfo.clear();
}

void VulkanResourceTracker::beginRenderPass(BeginRenderPassCommand* command)
{
    // do nothing.
}

void VulkanResourceTracker::setRenderPipeline(SetRenderPipelineCommand* command)
{
    // do nothing.
}

void VulkanResourceTracker::setVertexBuffer(SetVertexBufferCommand* command)
{
    // read
    {
        m_ongoingPassResourceInfo.read.buffers[command->buffer] = {
            VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
            VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
        };
    }
}

void VulkanResourceTracker::setIndexBuffer(SetIndexBufferCommand* command)
{
    // read
    {
        m_ongoingPassResourceInfo.read.buffers[command->buffer] = {
            VK_PIPELINE_STAGE_VERTEX_INPUT_BIT,
            VK_ACCESS_INDEX_READ_BIT,
        };
    }
}

void VulkanResourceTracker::setViewport(SetViewportCommand* command)
{
    // do nothing.
}

void VulkanResourceTracker::setScissor(SetScissorCommand* command)
{
    // do nothing.
}

void VulkanResourceTracker::setBlendConstant(SetBlendConstantCommand* command)
{
    // do nothing.
}

void VulkanResourceTracker::draw(DrawCommand* command)
{
    // do nothing.
}

void VulkanResourceTracker::drawIndexed(DrawIndexedCommand* command)
{
    // do nothing.
}

void VulkanResourceTracker::beginOcclusionQuery(BeginOcclusionQueryCommand* command)
{
    // do nothing.
}

void VulkanResourceTracker::endOcclusionQuery(EndOcclusionQueryCommand* command)
{
    // do nothing.
}

void VulkanResourceTracker::endRenderPass(EndRenderPassCommand* command)
{
    m_passResourceInfos.push_back(std::move(m_ongoingPassResourceInfo));
    m_ongoingPassResourceInfo.clear();
}

void VulkanResourceTracker::setRenderBindingGroup(SetBindGroupCommand* command)
{
    // TODO
}

void VulkanResourceTracker::copyBufferToBuffer(CopyBufferToBufferCommand* command)
{
    // do nothing.
}

void VulkanResourceTracker::copyBufferToTexture(CopyBufferToTextureCommand* command)
{
    // do nothing.
}

void VulkanResourceTracker::copyTextureToBuffer(CopyTextureToBufferCommand* command)
{
    // do nothing.
}

void VulkanResourceTracker::copyTextureToTexture(CopyTextureToTextureCommand* command)
{
    // do nothing.
}

void VulkanResourceTracker::resolveQuerySet(ResolveQuerySetCommand* command)
{
    // do nothing.
}

} // namespace jipu