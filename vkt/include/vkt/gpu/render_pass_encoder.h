#pragma once

#include "export.h"
#include <optional>
#include <vector>

namespace vkt
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

struct RenderPassEncoderDescriptor
{
    std::vector<ColorAttachment> colorAttachments{};
    std::optional<DepthStencilAttachment> depthStencilAttachment = std::nullopt;
    uint32_t sampleCount = 0;
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
    Pipeline* m_pipeline = nullptr;

    RenderPassEncoderDescriptor m_descriptor{};
};

} // namespace vkt