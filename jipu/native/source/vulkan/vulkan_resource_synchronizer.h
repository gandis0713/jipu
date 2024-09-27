#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "vulkan_api.h"
#include "vulkan_command.h"
#include "vulkan_resource_tracker.h"

namespace jipu
{

class Buffer;
class Texture;
class BindingGroup;
class VulkanCommandBuffer;

struct VulkanResourceSynchronizerDescriptor
{
    std::vector<PassResourceInfo> passResourceInfos{};
};

class VulkanResourceSynchronizer final
{
public:
    VulkanResourceSynchronizer() = default;
    VulkanResourceSynchronizer(VulkanCommandBuffer* commandBuffer, const VulkanResourceSynchronizerDescriptor& descriptor);
    ~VulkanResourceSynchronizer() = default;

public:
public:
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
    bool findSrcBuffer(Buffer* buffer) const;
    bool findSrcTexture(Texture* texture) const;
    BufferUsageInfo extractSrcBufferUsageInfo(Buffer* buffer);
    TextureUsageInfo extractSrcTextureUsageInfo(Texture* texture);

    void increasePassIndex();
    int32_t currentPassIndex() const;
    PassResourceInfo& getCurrentPassResourceInfo();

private:
    void sync();

private:
    struct PipelineBarrier
    {
        VkPipelineStageFlags srcStageMask;
        VkPipelineStageFlags dstStageMask;
        VkDependencyFlags dependencyFlags;
        std::vector<VkMemoryBarrier> memoryBarriers{};
        std::vector<VkBufferMemoryBarrier> bufferMemoryBarriers{};
        std::vector<VkImageMemoryBarrier> imageMemoryBarriers{};
    };

    void cmdPipelineBarrier(const PipelineBarrier& barrier);

private:
    VulkanCommandBuffer* m_commandBuffer = nullptr;
    VulkanResourceSynchronizerDescriptor m_descriptor{};
    int32_t m_currentPassIndex = -1;

    struct
    {
        std::unordered_set<Buffer*> buffers{};
        std::unordered_set<Texture*> textures{};
        void clear()
        {
            buffers.clear();
            textures.clear();
        }
    } m_activeResource;
};

} // namespace jipu