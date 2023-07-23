#pragma once

#include "export.h"

#include "vkt/gpu/command_buffer.h"
#include "vkt/gpu/swapchain.h"

#include <stdint.h>
#include <vector>

namespace vkt
{

struct QueueFlagBits
{
    static constexpr uint8_t kUndefined = 1 << 0; // 0x00000000
    static constexpr uint8_t kGraphics = 1 << 1;  // 0x00000001
    static constexpr uint8_t kCompute = 1 << 2;   // 0x00000002
    static constexpr uint8_t kTransfer = 1 << 3;  // 0x00000004
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

    virtual void submit(std::vector<CommandBuffer*> commandBuffers) = 0;
    virtual void submit(std::vector<CommandBuffer*> commandBuffers, Swapchain* swapchain) = 0;

public:
    QueueFlags getFlags() const;

protected:
    Device* m_device;

    QueueFlags m_flags = 0u;
};

} // namespace vkt