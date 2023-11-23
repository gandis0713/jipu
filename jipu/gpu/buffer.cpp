#include "jipu/gpu/buffer.h"
#include "jipu/gpu/device.h"

namespace jipu
{

Buffer::Buffer(Device* device, const BufferDescriptor& descriptor)
    : m_device(device)
    , m_descriptor(descriptor)
{
}

BufferUsageFlags Buffer::getUsage() const
{
    return m_descriptor.usage;
}

uint64_t Buffer::getSize() const
{
    return m_descriptor.size;
}
} // namespace jipu