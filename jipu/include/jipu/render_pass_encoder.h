#pragma once

#include "export.h"
#include <optional>
#include <vector>

namespace jipu
{

class Pipeline;
class Buffer;
class CommandBuffer;
class BindingGroup;
class TextureView;

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

union ColorClearValue {
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
    float depth = 1.0f;
    uint32_t stencil = 0;
};

struct DepthStencilAttachment
{
    TextureView* textureView = nullptr;
    LoadOp depthLoadOp = LoadOp::kDontCare;
    StoreOp depthStoreOp = StoreOp::kDontCare;
    LoadOp stencilLoadOp = LoadOp::kDontCare;
    StoreOp stencilStoreOp = StoreOp::kDontCare;
    DepthStencilClearValue clearValue{};
};

struct RenderPassDescriptor
{
    std::vector<ColorAttachment> colorAttachments{};
    std::optional<DepthStencilAttachment> depthStencilAttachment = std::nullopt;
    uint32_t sampleCount = 0;
};

enum IndexFormat
{
    kUint16 = 0,
    kUint32,
};

class JIPU_EXPORT RenderPassEncoder
{
public:
    RenderPassEncoder() = delete;
    RenderPassEncoder(CommandBuffer* commandBuffer, const RenderPassDescriptor& descriptor);
    virtual ~RenderPassEncoder() = default;

    virtual void setPipeline(Pipeline* pipeline) = 0;
    virtual void setBindingGroup(uint32_t index, BindingGroup* bindingGroup, std::vector<uint32_t> dynamicOffset = {}) = 0;

    virtual void setVertexBuffer(uint32_t slot, Buffer* buffer) = 0;
    virtual void setIndexBuffer(Buffer* buffer, IndexFormat format) = 0;

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
    virtual void drawIndexed(uint32_t indexCount,
                             uint32_t instanceCount,
                             uint32_t indexOffset,
                             uint32_t vertexOffset,
                             uint32_t firstInstance) = 0;

    virtual void end() = 0;

protected:
    CommandBuffer* m_commandBuffer = nullptr;
    Pipeline* m_pipeline = nullptr;

    const RenderPassDescriptor m_descriptor{};
};

} // namespace jipu