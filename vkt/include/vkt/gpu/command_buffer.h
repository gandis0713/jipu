#pragma once

#include "vkt/gpu/buffer.h"

namespace vkt
{

enum class CommandType
{
    kIndexBuffer,
    kVertexBuffer,
    kCopyBufferToBuffer,
    kCopyBufferToTexture,
};

struct CopyBufferToBuffer
{
    Buffer* src;
    Buffer* dst;
    uint64_t srcOffset;
    uint64_t dstOffset;
    uint64_t size;
};

struct CommandBufferDescriptor
{
};

class CommandEncoder;
class CommandBuffer
{
public:
    CommandBuffer() = delete;
    CommandBuffer(CommandEncoder* encoder, const CommandBufferDescriptor& descriptor);
    virtual ~CommandBuffer() = default;

protected:
    CommandEncoder* m_encoder = nullptr;
};
} // namespace vkt