#pragma once

namespace vkt
{

struct QueueDescriptor
{
};

class Device;
class Queue
{
public:
    Queue() = delete;
    Queue(Device* device, const QueueDescriptor& descriptor);
    virtual ~Queue() = default;

protected:
    Device* m_device;
};

} // namespace vkt