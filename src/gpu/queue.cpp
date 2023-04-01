#include "gpu/queue.h"
#include "gpu/device.h"

namespace vkt
{

Queue::Queue(Device* device, const QueueDescriptor& descriptor)
    : m_device(device)
{
}

} // namespace vkt