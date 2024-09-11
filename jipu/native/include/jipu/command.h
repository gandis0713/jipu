#pragma once

#include "binding_group.h"
#include "buffer.h"
#include "pipeline.h"
#include "query_set.h"
#include "render_pass_encoder.h"
#include "texture_view.h"

namespace jipu
{
enum class CommandType
{
    kBeginComputePass,
    kEndComputePass,
    kBeginRenderPass,
    kEndRenderPass,
    kClearBuffer,
    kCopyBufferToBuffer,
    kCopyBufferToTexture,
    kCopyTextureToBuffer,
    kCopyTextureToTexture,
    kDispatch,
    kDispatchIndirect,
    kDraw,
    kDrawIndexed,
    kDrawIndirect,
    kDrawIndexedIndirect,
    kBeginOcclusionQuery,
    kEndOcclusionQuery,
    kResolveQuerySet,
    kSetComputePipeline,
    kSetRenderPipeline,
    kSetViewport,
    kSetScissor,
    kSetBlendConstant,
    kSetBindGroup,
    kSetIndexBuffer,
    kSetVertexBuffer,
    kWriteTimestamp
};

struct Command
{
    CommandType type;
};

struct BeginComputePassCommand : public Command
{
};

struct EndComputePassCommand : public Command
{
};

struct BeginRenderPassCommand : public Command
{
    std::vector<ColorAttachment> colorAttachments{};
    std::optional<DepthStencilAttachment> depthStencilAttachment = std::nullopt;
    QuerySet* occlusionQuerySet = nullptr;
    RenderPassTimestampWrites timestampWrites{};
};

struct EndRenderPassCommand : public Command
{
};

struct ClearBufferCommand : public Command
{
};

struct CopyBufferToBufferCommand : public Command
{
};

struct CopyBufferToTextureCommand : public Command
{
};

struct CopyTextureToBufferCommand : public Command
{
};

struct CopyTextureToTextureCommand : public Command
{
};

struct DispatchCommand : public Command
{
};

struct DispatchIndirectCommand : public Command
{
};

struct DrawCommand : public Command
{
    uint32_t vertexCount = 0;
    uint32_t instanceCount = 0;
    uint32_t firstVertex = 0;
    uint32_t firstInstance = 0;
};

struct DrawIndexedCommand : public Command
{
    uint32_t indexCount = 0;
    uint32_t instanceCount = 0;
    uint32_t indexOffset = 0;
    uint32_t vertexOffset = 0;
    uint32_t firstInstance = 0;
};

struct DrawIndirectCommand : public Command
{
};

struct DrawIndexedIndirectCommand : public Command
{
};

struct BeginOcclusionQueryCommand : public Command
{
    QuerySet* querySet = nullptr;
    uint32_t queryIndex = 0;
};

struct EndOcclusionQueryCommand : public Command
{
    QuerySet* querySet = nullptr;
};

struct ResolveQuerySetCommand : public Command
{
};

struct SetComputePipelineCommand : public Command
{
};

struct SetRenderPipelineCommand : public Command
{
    RenderPipeline* pipeline = nullptr;
};

struct SetViewportCommand : public Command
{
    float x = 0;
    float y = 0;
    float width = 0;
    float height = 0;
    float minDepth = 0;
    float maxDepth = 0;
};

struct SetScissorCommand : public Command
{
    float x = 0;
    float y = 0;
    float width = 0;
    float height = 0;
};

struct SetBlendConstantCommand : public Command
{
    Color color{};
};

struct SetBindGroupCommand : public Command
{
    uint32_t index = 0;
    BindingGroup* bindingGroup = nullptr;
    std::vector<uint32_t> dynamicOffset{};
};

struct SetIndexBufferCommand : public Command
{
    Buffer* buffer = nullptr;
    IndexFormat format = IndexFormat::kUndefined;
};

struct SetVertexBufferCommand : public Command
{
    uint32_t slot = 0;
    Buffer* buffer = nullptr;
};

struct WriteTimestampCommand : public Command
{
};

} // namespace jipu