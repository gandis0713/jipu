#include "vkt/gpu/command_buffer.h"
#include "vkt/gpu/device.h"

namespace vkt
{

CommandBuffer::CommandBuffer(Device* device, const CommandBufferDescriptor& descriptor)
    : m_device(device)
{
}

Device* CommandBuffer::getDevice() const
{
    return m_device;
}

} // namespace vkt