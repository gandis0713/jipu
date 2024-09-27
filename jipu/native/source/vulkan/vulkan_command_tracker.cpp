#include "vulkan_command_tracker.h"

namespace jipu
{

void VulkanCommandTracker::beginComputePass(BeginComputePassCommand* command)
{
    // do nothing.
}

void VulkanCommandTracker::setComputePipeline(SetComputePipelineCommand* command)
{
    // do nothing.
}

void VulkanCommandTracker::setComputeBindingGroup(SetBindGroupCommand* command)
{
    // do nothing.
}

void VulkanCommandTracker::dispatch(DispatchCommand* command)
{
    // do nothing.
}

void VulkanCommandTracker::dispatchIndirect(DispatchIndirectCommand* command)
{
    // TODO
}

void VulkanCommandTracker::endComputePass(EndComputePassCommand* command)
{
    // do nothing.
}

void VulkanCommandTracker::beginRenderPass(BeginRenderPassCommand* command)
{
    // TODO
}

void VulkanCommandTracker::setRenderPipeline(SetRenderPipelineCommand* command)
{
    // do nothing.
}

void VulkanCommandTracker::setVertexBuffer(SetVertexBufferCommand* command)
{
    // do nothing.
}

void VulkanCommandTracker::setIndexBuffer(SetIndexBufferCommand* command)
{
    // do nothing.
}

void VulkanCommandTracker::setViewport(SetViewportCommand* command)
{
    // do nothing.
}

void VulkanCommandTracker::setScissor(SetScissorCommand* command)
{
    // do nothing.
}

void VulkanCommandTracker::setBlendConstant(SetBlendConstantCommand* command)
{
    // do nothing.
}

void VulkanCommandTracker::draw(DrawCommand* command)
{
    // do nothing.
}

void VulkanCommandTracker::drawIndexed(DrawIndexedCommand* command)
{
    // do nothing.
}

void VulkanCommandTracker::beginOcclusionQuery(BeginOcclusionQueryCommand* command)
{
    // do nothing.
}

void VulkanCommandTracker::endOcclusionQuery(EndOcclusionQueryCommand* command)
{
    // do nothing.
}

void VulkanCommandTracker::endRenderPass(EndRenderPassCommand* command)
{
}

void VulkanCommandTracker::setRenderBindingGroup(SetBindGroupCommand* command)
{
}

void VulkanCommandTracker::copyBufferToBuffer(CopyBufferToBufferCommand* command)
{
    // do nothing.
}

void VulkanCommandTracker::copyBufferToTexture(CopyBufferToTextureCommand* command)
{
    // do nothing.
}

void VulkanCommandTracker::copyTextureToBuffer(CopyTextureToBufferCommand* command)
{
    // do nothing.
}

void VulkanCommandTracker::copyTextureToTexture(CopyTextureToTextureCommand* command)
{
    // do nothing.
}

void VulkanCommandTracker::resolveQuerySet(ResolveQuerySetCommand* command)
{
    // do nothing.
}

} // namespace jipu