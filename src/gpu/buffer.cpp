#include "vkt/gpu/buffer.h"
#include "vkt/gpu/device.h"

namespace vkt
{

Buffer::Buffer(Device* device, const BufferDescriptor& descriptor)
    : m_device(device)
{
}
} // namespace vkt