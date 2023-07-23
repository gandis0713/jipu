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
    TextureView* textureView = nullptr;
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
class VKT_EXPORT CommandEncoder
{
public:
    CommandEncoder() = delete;
    CommandEncoder(CommandBuffer* commandBuffer);
    virtual ~CommandEncoder() = default;

    virtual void begin() = 0;
    virtual void end() = 0;

    CommandBuffer* getCommandBuffer() const;

protected:
    CommandBuffer* m_commandBuffer = nullptr;
    Pipeline* m_pipeline = nullptr;
};

struct RenderCommandEncoderDescriptor
{
    std::vector<ColorAttachment> colorAttachments{};
    DepthStencilAttachment depthStencilAttachment{};
};

class VKT_EXPORT RenderCommandEncoder : public CommandEncoder
{
public:
    RenderCommandEncoder() = delete;
    RenderCommandEncoder(CommandBuffer* commandBuffer, const RenderCommandEncoderDescriptor& descriptor);
    virtual ~RenderCommandEncoder() = default;

    virtual void setPipeline(Pipeline* pipeline) = 0;
    virtual void setBindingGroup(uint32_t index, BindingGroup* bindingGroup) = 0;

    virtual void setVertexBuffer(Buffer* buffer) = 0;
    virtual void setIndexBuffer(Buffer* buffer) = 0;

    virtual void draw(uint32_t vertexCount) = 0;
    virtual void drawIndexed(uint32_t indexCount) = 0;

    // state
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

protected:
    RenderCommandEncoderDescriptor m_descriptor{};
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

struct BlitCommandEncoderDescriptor
{
};

class VKT_EXPORT BlitCommandEncoder : public CommandEncoder
{
public:
    BlitCommandEncoder() = delete;
    BlitCommandEncoder(CommandBuffer* commandBuffer, const BlitCommandEncoderDescriptor& descriptor);
    virtual ~BlitCommandEncoder() = default;

    virtual void copyBufferToBuffer(const BlitBuffer& src, const BlitBuffer& dst, uint64_t size) = 0;
    virtual void copyBufferToTexture(const BlitTextureBuffer& buffer, const BlitTexture& texture, const Extent3D& extent) = 0;

    virtual void copyTextureToBuffer() = 0;  // TODO: not yet implemented
    virtual void copyTextureToTexture() = 0; // TODO: not yet implemented
};

} // namespace vkt