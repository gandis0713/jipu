#include "vkt/gpu/buffer.h"
#include "vkt/gpu/device.h"

namespace vkt
{

Buffer::Buffer(Device* device, const BufferDescriptor& descriptor)
    : m_device(device)
    , m_size(descriptor.size)
{
}

uint64_t Buffer::getSize() const
{
    return m_size;
}
} // namespace vkt