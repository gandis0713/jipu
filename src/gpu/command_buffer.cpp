#include "jipu/gpu/command_buffer.h"
#include "jipu/gpu/device.h"

namespace jipu
{

CommandBuffer::CommandBuffer(Device* device, const CommandBufferDescriptor& descriptor)
    : m_device(device)
    , m_descriptor(descriptor)
{
}

CommandBufferUsage CommandBuffer::getUsage() const
{
    return m_descriptor.usage;
}

Device* CommandBuffer::getDevice() const
{
    return m_device;
}

} // namespace jipu