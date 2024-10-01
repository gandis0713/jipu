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
    struct Source // consumer
    {
        std::unordered_map<Buffer*, BufferUsageInfo> buffers;
        std::unordered_map<Texture*, TextureUsageInfo> textures;
    } dst;

    struct Destination // producer
    {
        std::unordered_map<Buffer*, BufferUsageInfo> buffers;
        std::unordered_map<Texture*, TextureUsageInfo> textures;
    } src;

    void clear()
    {
        dst.buffers.clear();
        dst.textures.clear();

        src.buffers.clear();
        src.textures.clear();
    }
};

struct VulkanCommandRecordResult
{
    VkCommandBuffer commandBuffer = VK_NULL_HANDLE;
    CommandResourceInfo resourceInfo{};
};

struct VulkanCommandRecorderDecsriptor
{
    VulkanCommandBuffer* commandBuffer = nullptr;
};

class VULKAN_EXPORT VulkanCommandRecorder final
{
public:
    VulkanCommandRecorder() = delete;
    VulkanCommandRecorder(VulkanDevice* device, const VulkanCommandRecorderDecsriptor& descriptor);
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

public:
    VulkanDevice* getDevice() const;

public:
    VkCommandBuffer getVkCommandBuffer() const;

private:
    VulkanCommandRecordResult result();

private:
    VulkanDevice* m_device = nullptr;
    VulkanCommandRecorderDecsriptor m_descriptor{};
    VulkanCommandResourceSynchronizer m_commandResourceSyncronizer{};

private:
    VulkanRenderPipeline* m_renderPipeline = nullptr;
    VulkanComputePipeline* m_computePipeline = nullptr;

private:
    VkCommandBuffer m_vkcommandBuffer = VK_NULL_HANDLE;
};

// Generator
VkPipelineStageFlags generatePipelineStageFlags(Command* cmd);
VkAccessFlags generateBufferAccessFlags(BufferUsageFlags usage);
VkAccessFlags generateTextureAccessFlags(TextureUsageFlags usage);

} // namespace jipu