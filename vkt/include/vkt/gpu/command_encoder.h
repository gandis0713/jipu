#pragma once

#include "export.h"
#include "vkt/gpu/texture.h"
#include "vkt/gpu/texture_view.h"

#include <vector>

namespace vkt
{

enum class LoadOp : uint8_t
{
    kDontCare = 0,
    kLoad,
    kClear,
};

enum class StoreOp : uint8_t
{
    kDontCare = 0,
    kStore,
};

union ColorClearValue
{
    float float32[4];
    int32_t int32[4];
    uint32_t uint32[4];
};

struct ColorAttachment
{
    TextureView* renderView = nullptr;
    TextureView* resolveView = nullptr;
    LoadOp loadOp = LoadOp::kDontCare;
    StoreOp storeOp = StoreOp::kDontCare;
    ColorClearValue clearValue{};
};

struct DepthStencilClearValue
{
    float depth;
    uint32_t stencil;
};

struct DepthStencilAttachment
{
    TextureView* textureView = nullptr;
    LoadOp loadOp = LoadOp::kDontCare;
    StoreOp storeOp = StoreOp::kDontCare;
    DepthStencilClearValue clearValue{};
};

class Pipeline;
class Buffer;
class CommandBuffer;
class BindingGroup;
class Buffer;
class Texture;

struct RenderPassEncoderDescriptor
{
    std::vector<ColorAttachment> colorAttachments{};
    DepthStencilAttachment depthStencilAttachment{};
};

class VKT_EXPORT RenderPassEncoder
{
public:
    RenderPassEncoder() = delete;
    RenderPassEncoder(CommandBuffer* commandBuffer, const RenderPassEncoderDescriptor& descriptor);
    virtual ~RenderPassEncoder() = default;

    virtual void setPipeline(Pipeline* pipeline) = 0;
    virtual void setBindingGroup(uint32_t index, BindingGroup* bindingGroup) = 0;

    virtual void setVertexBuffer(Buffer* buffer) = 0;
    virtual void setIndexBuffer(Buffer* buffer) = 0;

    virtual void setViewport(float x,
                             float y,
                             float width,
                             float height,
                             float minDepth,
                             float maxDepth) = 0;

    virtual void setScissor(float x,
                            float y,
                            float width,
                            float height) = 0;

    virtual void draw(uint32_t vertexCount) = 0;
    virtual void drawIndexed(uint32_t indexCount) = 0;

    virtual void end() = 0;

protected:
    CommandBuffer* m_commandBuffer = nullptr;
    RenderPassEncoderDescriptor m_descriptor{};

protected:
    Pipeline* m_pipeline = nullptr;
};

struct BlitBuffer
{
    Buffer* buffer = nullptr;
    uint32_t offset = 0;
};

struct BlitTextureBuffer : BlitBuffer
{
    uint32_t bytesPerRow = 0;
    uint32_t rowsPerTexture = 0;
};

struct BlitTexture
{
    Texture* texture = nullptr;
};

struct CommandEncoderDescriptor
{
};

class VKT_EXPORT CommandEncoder
{
public:
    CommandEncoder() = delete;
    CommandEncoder(CommandBuffer* commandBuffer, const CommandEncoderDescriptor& descriptor);
    virtual ~CommandEncoder() = default;

    virtual std::unique_ptr<RenderPassEncoder> beginRenderPass(const RenderPassEncoderDescriptor& descriptor) = 0;

    virtual void copyBufferToBuffer(const BlitBuffer& src, const BlitBuffer& dst, uint64_t size) = 0;
    virtual void copyBufferToTexture(const BlitTextureBuffer& buffer, const BlitTexture& texture, const Extent3D& extent) = 0;

    virtual void copyTextureToBuffer() = 0;  // TODO: not yet implemented
    virtual void copyTextureToTexture() = 0; // TODO: not yet implemented

    virtual CommandBuffer* end() = 0;

public:
    CommandBuffer* getCommandBuffer() const;

protected:
    CommandBuffer* m_commandBuffer = nullptr;
};

} // namespace vkt