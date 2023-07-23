#pragma once

#include "export.h"
#include <stdint.h>

namespace vkt
{

struct BufferUsageFlagBits
{
    static constexpr uint32_t kUndefined = 1 << 0; // 0x00000000
    static constexpr uint32_t kMapRead = 1 << 1;   // 0x00000001
    static constexpr uint32_t kMapWrite = 1 << 2;  // 0x00000002
    static constexpr uint32_t kIndex = 1 << 3;     // 0x00000004
    static constexpr uint32_t kVertex = 1 << 4;    // 0x00000008
    static constexpr uint32_t kUniform = 1 << 5;   // 0x00000010
    static constexpr uint32_t kCopySrc = 1 << 6;   // 0x00000020
    static constexpr uint32_t kCopyDst = 1 << 7;   // 0x00000040
};
using BufferUsageFlags = uint32_t;

struct BufferDescriptor
{
    uint64_t size = 0;
    BufferUsageFlags usage = BufferUsageFlagBits::kUndefined;
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
