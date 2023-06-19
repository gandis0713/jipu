#pragma once

#include "export.h"
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
class VKT_EXPORT CommandEncoder
{
public:
    CommandEncoder() = delete;
    CommandEncoder(CommandBuffer* commandBuffer);
    virtual ~CommandEncoder() = default;

    virtual void begin() = 0;
    virtual void end() = 0;

    virtual void setPipeline(Pipeline* pipeline) = 0;

    CommandBuffer* getCommandBuffer() const;

protected:
    CommandBuffer* m_commandBuffer = nullptr;
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

    virtual void setVertexBuffer(Buffer* buffer) = 0;
    virtual void setIndexBuffer(Buffer* buffer) = 0;

    virtual void setViewport(float x,
                             float y,
                             float width,
                             float height,
                             float minDepth,
                             float maxDepth) = 0;

    virtual void draw(uint32_t vertexCount) = 0;
    virtual void drawIndexed(uint32_t indexCount) = 0;

protected:
    RenderCommandEncoderDescriptor m_descriptor{};
};
} // namespace vkt