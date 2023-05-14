#pragma once

#include <stdint.h>
#include <vkt/gpu/buffer.h>

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

} // namespace vkt