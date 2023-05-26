#pragma once

#include "export.h"
#include <stdint.h>

namespace vkt
{

enum BufferFlagBits : uint32_t
{
    kInvalid = 1 << 0,  // 0x00000000
    kMapRead = 1 << 1,  // 0x00000001
    kMapWrite = 1 << 2, // 0x00000002
    kIndex = 1 << 3,    // 0x00000004
    kVertex = 1 << 4,   // 0x00000008
    kUniform = 1 << 5,  // 0x00000010
    kCopySrc = 1 << 6,  // 0x00000020
    kCopyDst = 1 << 7,  // 0x00000040
};
using BufferFlags = uint32_t;

struct BufferDescriptor
{
    uint64_t size = 0;
    BufferFlags flags = BufferFlagBits::kInvalid;
};

class Device;
class VKT_EXPORT Buffer
{
public:
    Buffer() = delete;
    Buffer(Device* device, const BufferDescriptor& descriptor);
    virtual ~Buffer() = default;

    virtual void* map() = 0;
    virtual void unmap() = 0;

    uint64_t getSize() const;

protected:
    Device* m_device = nullptr;
    uint64_t m_size{ 0 };
};

} // namespace vkt