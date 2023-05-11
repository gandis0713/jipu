#pragma once

#include "export.h"
#include <stdint.h>

namespace vkt
{

struct BufferDescriptor
{
    uint64_t size = 0;
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
    Device* m_device{ nullptr };
    uint64_t m_size{ 0 };
};
} // namespace vkt