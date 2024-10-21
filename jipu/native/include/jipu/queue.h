#pragma once

#include "export.h"

#include "jipu/command_buffer.h"
#include "jipu/swapchain.h"

#include <functional>
#include <stdint.h>
#include <vector>

namespace jipu
{

struct QueueDescriptor
{
};

class JIPU_EXPORT Queue
{
public:
    virtual ~Queue() = default;

protected:
    Queue() = default;

public:
    virtual void submit(std::vector<CommandBuffer*> commandBuffers) = 0;
};

} // namespace jipu