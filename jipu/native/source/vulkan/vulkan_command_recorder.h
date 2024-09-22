#pragma once

#include "vulkan_api.h"
#include "vulkan_command.h"
#include "vulkan_export.h"

namespace jipu
{

class VulkanCommandBuffer;
class VulkanRenderPipeline;
class VULKAN_EXPORT VulkanCommandRecorder
{
public:
    VulkanCommandRecorder() = delete;
    VulkanCommandRecorder(VulkanCommandBuffer* commandBuffer);
    ~VulkanCommandRecorder() = default;

    VulkanCommandRecorder(const VulkanCommandRecorder&) = delete;
    VulkanCommandRecorder& operator=(const VulkanCommandRecorder&) = delete;

    void record();

public:
    VulkanCommandBuffer* getCommandBuffer() const;

private:
    void beginRecord();
    void endRecord();

    // compute pass
    void beginComputePass(BeginComputePassCommand* command);
    void setComputePipeline(SetComputePipelineCommand* command);
    void setComputeBindingGroup(SetBindGroupCommand* command);
    void dispatch(DispatchCommand* command);
    void dispatchIndirect(DispatchIndirectCommand* command);
    void endComputePass(EndComputePassCommand* command);

    // render pass
    void beginRenderPass(BeginRenderPassCommand* command);
    void setRenderPipeline(SetRenderPipelineCommand* command);
    void setRenderBindingGroup(SetBindGroupCommand* command);
    void setVertexBuffer(SetVertexBufferCommand* command);
    void setIndexBuffer(SetIndexBufferCommand* command);
    void setViewport(SetViewportCommand* command);
    void setScissor(SetScissorCommand* command);
    void setBlendConstant(SetBlendConstantCommand* command);
    void draw(DrawCommand* command);
    void drawIndexed(DrawIndexedCommand* command);
    void beginOcclusionQuery(BeginOcclusionQueryCommand* command);
    void endOcclusionQuery(EndOcclusionQueryCommand* command);
    void endRenderPass(EndRenderPassCommand* command);

    // common

    // copy
    void copyBufferToBuffer(CopyBufferToBufferCommand* command);
    void copyBufferToTexture(CopyBufferToTextureCommand* command);
    void copyTextureToBuffer(CopyTextureToBufferCommand* command);
    void copyTextureToTexture(CopyTextureToTextureCommand* command);

    // query
    void resolveQuerySet(ResolveQuerySetCommand* command);

private:
    VulkanCommandBuffer* m_commandBuffer = nullptr;
    Pipeline* m_pipeline = nullptr;
};

// Generator
VkPipelineStageFlags generatePipelineStageFlags(Command* cmd);
VkAccessFlags generateBufferAccessFlags(BufferUsageFlags usage);
VkAccessFlags generateTextureAccessFlags(TextureUsageFlags usage);

} // namespace jipu