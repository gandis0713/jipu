#pragma once

#include "vulkan_api.h"
#include "vulkan_command.h"
#include "vulkan_command_encoder.h"
#include "vulkan_command_resource_synchronizer.h"
#include "vulkan_export.h"

namespace jipu
{

class VulkanPipeline;
class VulkanCommandBuffer;
class VulkanRenderPipeline;
class VulkanComputePipeline;

struct CommandResourceInfo
{
    struct Source // producer
    {
        std::unordered_map<Buffer*, BufferUsageInfo> buffers;
        std::unordered_map<Texture*, TextureUsageInfo> textures;
    } src;

    struct Destination // consumer
    {
        std::unordered_map<Buffer*, BufferUsageInfo> buffers;
        std::unordered_map<Texture*, TextureUsageInfo> textures;
    } dst;

    void clear()
    {
        src.buffers.clear();
        src.textures.clear();

        dst.buffers.clear();
        dst.textures.clear();
    }
};

struct VulkanCommandRecordResult
{
    CommandBuffer* commandBuffer = nullptr;
    // resources info that not syncronized in command buffer.
    CommandResourceInfo resourceInfo{};
};

class VULKAN_EXPORT VulkanCommandRecorder final
{
public:
    VulkanCommandRecorder() = delete;
    VulkanCommandRecorder(VulkanCommandBuffer* commandBuffer);
    ~VulkanCommandRecorder();

    VulkanCommandRecorder(const VulkanCommandRecorder&) = delete;
    VulkanCommandRecorder& operator=(const VulkanCommandRecorder&) = delete;

    VulkanCommandRecordResult record();

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

    // copy
    void copyBufferToBuffer(CopyBufferToBufferCommand* command);
    void copyBufferToTexture(CopyBufferToTextureCommand* command);
    void copyTextureToBuffer(CopyTextureToBufferCommand* command);
    void copyTextureToTexture(CopyTextureToTextureCommand* command);

    // query
    void resolveQuerySet(ResolveQuerySetCommand* command);

private:
    VulkanCommandRecordResult result();

private:
    VulkanCommandBuffer* m_commandBuffer = nullptr;
    VulkanCommandResourceSynchronizer m_commandResourceSyncronizer{};

private:
    VulkanRenderPipeline* m_renderPipeline = nullptr;
    VulkanComputePipeline* m_computePipeline = nullptr;
};

// Generator
VkPipelineStageFlags generatePipelineStageFlags(Command* cmd);
VkAccessFlags generateBufferAccessFlags(BufferUsageFlags usage);
VkAccessFlags generateTextureAccessFlags(TextureUsageFlags usage);

} // namespace jipu