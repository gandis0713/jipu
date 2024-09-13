#pragma once

#include "jipu/command.h"
#include "vulkan_api.h"
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

    void beginRecord();
    void endRecord();

    // compute pass
    void beginComputePass(BeginComputePassCommand* command);
    void setComputePipeline(SetComputePipelineCommand* command);
    void dispatch(DispatchCommand* command);
    void endComputePass(EndComputePassCommand* command);

    // render pass
    void beginRenderPass(BeginRenderPassCommand* command);
    void setRenderPipeline(SetRenderPipelineCommand* command);
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
    void setBindingGroup(SetBindGroupCommand* command);

    // copy
    void copyBufferToBuffer(CopyBufferToBufferCommand* command);
    void copyBufferToTexture(CopyBufferToTextureCommand* command);
    void copyTextureToBuffer(CopyTextureToBufferCommand* command);
    void copyTextureToTexture(CopyTextureToTextureCommand* command);
    void resolveQuerySet(ResolveQuerySetCommand* command);

public:
    VulkanCommandBuffer* getCommandBuffer() const;

private:
    VulkanCommandBuffer* m_commandBuffer = nullptr;
    Pipeline* m_pipeline = nullptr;
};

} // namespace jipu