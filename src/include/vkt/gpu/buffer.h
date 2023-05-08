#pragma once

#include "export.h"
#include <stdint.h>

namespace vkt
{

struct BufferDescriptor
{
    uint32_t size = 0;
};

class Device;
class VKT_EXPORT Buffer
{
public:
    Buffer() = delete;
    Buffer(Device* device, const BufferDescriptor& descriptor);
    virtual ~Buffer() = default;

protected:
    Device* m_device{ nullptr };
};
} // namespace vkt