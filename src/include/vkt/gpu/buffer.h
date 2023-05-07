#pragma once

#include "export.h"

namespace vkt
{
struct BufferDescriptor
{
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