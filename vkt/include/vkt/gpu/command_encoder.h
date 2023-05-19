#pragma once

#include "export.h"
#include "vkt/gpu/texture_view.h"

namespace vkt
{

struct CommandEncoderDescriptor
{
    TextureView* textureView = nullptr;
};

class Pipeline;
class Buffer;
class CommandBuffer;
class VKT_EXPORT CommandEncoder
{
public:
    CommandEncoder() = delete;
    CommandEncoder(CommandBuffer* commandBuffer, const CommandEncoderDescriptor& descriptor);
    virtual ~CommandEncoder() = default;

    virtual void startEncoding() = 0;
    virtual void endEncoding() = 0;

    virtual void setPipeline(Pipeline* pipeline) = 0;
    virtual void setVertexBuffer(Buffer* buffer) = 0;
    virtual void setIndexBuffer(Buffer* buffer) = 0;

protected:
    CommandBuffer* m_commandBuffer = nullptr;
    CommandEncoderDescriptor m_descriptor{};
};
} // namespace vkt