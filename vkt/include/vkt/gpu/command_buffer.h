#pragma once

#include "export.h"
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

class Device;
class CommandBuffer
{
public:
    CommandBuffer() = delete;
    CommandBuffer(Device* device, const CommandBufferDescriptor& descriptor);
    virtual ~CommandBuffer() = default;

protected:
    Device* m_device = nullptr;
};
} // namespace vkt