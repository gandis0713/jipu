#include "vkt/gpu/queue.h"
#include "vkt/gpu/device.h"

namespace vkt
{

Queue::Queue(Device* device, const QueueDescriptor& descriptor)
    : m_device(device)
{
}

QueueType Queue::getType() const
{
    return m_type;
}

} // namespace vkt