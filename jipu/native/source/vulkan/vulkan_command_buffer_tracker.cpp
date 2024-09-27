#include "vulkan_command_buffer_tracker.h"

namespace jipu
{

void VulkanCommandBufferTracker::beginComputePass(BeginComputePassCommand* command)
{
    // do nothing.
}

void VulkanCommandBufferTracker::setComputePipeline(SetComputePipelineCommand* command)
{
    // do nothing.
}

void VulkanCommandBufferTracker::setComputeBindingGroup(SetBindGroupCommand* command)
{
    // do nothing.
}

void VulkanCommandBufferTracker::dispatch(DispatchCommand* command)
{
    // do nothing.
}

void VulkanCommandBufferTracker::dispatchIndirect(DispatchIndirectCommand* command)
{
    // TODO
}

void VulkanCommandBufferTracker::endComputePass(EndComputePassCommand* command)
{
    // do nothing.
}

void VulkanCommandBufferTracker::beginRenderPass(BeginRenderPassCommand* command)
{
    // TODO
}

void VulkanCommandBufferTracker::setRenderPipeline(SetRenderPipelineCommand* command)
{
    // do nothing.
}

void VulkanCommandBufferTracker::setVertexBuffer(SetVertexBufferCommand* command)
{
    // do nothing.
}

void VulkanCommandBufferTracker::setIndexBuffer(SetIndexBufferCommand* command)
{
    // do nothing.
}

void VulkanCommandBufferTracker::setViewport(SetViewportCommand* command)
{
    // do nothing.
}

void VulkanCommandBufferTracker::setScissor(SetScissorCommand* command)
{
    // do nothing.
}

void VulkanCommandBufferTracker::setBlendConstant(SetBlendConstantCommand* command)
{
    // do nothing.
}

void VulkanCommandBufferTracker::draw(DrawCommand* command)
{
    // do nothing.
}

void VulkanCommandBufferTracker::drawIndexed(DrawIndexedCommand* command)
{
    // do nothing.
}

void VulkanCommandBufferTracker::beginOcclusionQuery(BeginOcclusionQueryCommand* command)
{
    // do nothing.
}

void VulkanCommandBufferTracker::endOcclusionQuery(EndOcclusionQueryCommand* command)
{
    // do nothing.
}

void VulkanCommandBufferTracker::endRenderPass(EndRenderPassCommand* command)
{
}

void VulkanCommandBufferTracker::setRenderBindingGroup(SetBindGroupCommand* command)
{
}

void VulkanCommandBufferTracker::copyBufferToBuffer(CopyBufferToBufferCommand* command)
{
    // do nothing.
}

void VulkanCommandBufferTracker::copyBufferToTexture(CopyBufferToTextureCommand* command)
{
    // do nothing.
}

void VulkanCommandBufferTracker::copyTextureToBuffer(CopyTextureToBufferCommand* command)
{
    // do nothing.
}

void VulkanCommandBufferTracker::copyTextureToTexture(CopyTextureToTextureCommand* command)
{
    // do nothing.
}

void VulkanCommandBufferTracker::resolveQuerySet(ResolveQuerySetCommand* command)
{
    // do nothing.
}

} // namespace jipu