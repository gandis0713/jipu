#pragma once

#include "export.h"

namespace vkt
{

struct CommandEncoderDescriptor
{
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

    virtual void setPipeline(Pipeline* pipeline) = 0;
    virtual void setVertexBuffer(Buffer* buffer) = 0;
    virtual void setIndexBuffer(Buffer* buffer) = 0;

protected:
    CommandBuffer* m_commandBuffer = nullptr;
};
} // namespace vkt