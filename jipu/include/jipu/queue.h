#pragma once

#include "export.h"

#include "jipu/command_buffer.h"
#include "jipu/swapchain.h"

#include <functional>
#include <stdint.h>
#include <vector>

namespace jipu
{

using CommandBufferRef = std::reference_wrapper<CommandBuffer>;
using CommandBuffers = std::vector<CommandBufferRef>;

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

class JIPU_EXPORT Queue
{
public:
    virtual ~Queue() = default;

protected:
    Queue() = default;

public:
    virtual void submit(CommandBuffers commandBuffers) = 0;
    virtual void submit(CommandBuffers commandBuffers, Swapchain* swapchain) = 0;
};

} // namespace jipu