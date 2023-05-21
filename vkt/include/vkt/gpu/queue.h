#pragma once

#include "export.h"

namespace vkt
{

enum class QueueType
{
    kUndefined = 0,
    kGraphics,
    kCompute,
    kTransfer,
};

struct QueueDescriptor
{
    QueueType type;
};

class Device;
class VKT_EXPORT Queue
{
public:
    Queue() = delete;
    Queue(Device* device, const QueueDescriptor& descriptor);
    virtual ~Queue() = default;

    virtual void submit() = 0;

public:
    QueueType getType() const;

protected:
    Device* m_device;

    QueueType m_type = QueueType::kUndefined;
};

} // namespace vkt