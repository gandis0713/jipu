#pragma once

#include "export.h"

#include "vkt/gpu/command_buffer.h"
#include "vkt/gpu/swapchain.h"

#include <stdint.h>

namespace vkt
{

enum QueueFlagBits : uint8_t
{
    // 0x00000000 is undefined,
    kGraphics = 0x00000001,
    kCompute = 0x00000002,
    kTransfer = 0x00000004,
};
using QueueFlags = uint8_t;

struct QueueDescriptor
{
    QueueFlags flags;
};

class Device;
class VKT_EXPORT Queue
{
public:
    Queue() = delete;
    Queue(Device* device, const QueueDescriptor& descriptor);
    virtual ~Queue() = default;

    virtual void submit(CommandBuffer* commandBuffer) = 0;

public:
    QueueFlags getFlags() const;

protected:
    Device* m_device;

    QueueFlags m_flags = 0u;
};

} // namespace vkt