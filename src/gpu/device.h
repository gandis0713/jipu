#pragma once

#include "gpu/pipeline.h"
#include "gpu/queue.h"
#include "gpu/swapchain.h"

#include <memory>

namespace vkt
{

class Adapter;

struct DeviceDescriptor
{
};

class Device
{
public:
    Device() = delete;
    Device(Adapter* adapter, DeviceDescriptor descriptor);
    virtual ~Device() = default;

    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;

    virtual std::unique_ptr<SwapChain> createSwapChain(SwapChainDescriptor&& descriptor) = 0;
    virtual std::unique_ptr<Pipeline> createPipeline(PipelineDescriptor descriptor) = 0;
    virtual std::unique_ptr<Queue> createQueue(const QueueDescriptor& descriptor) = 0;

protected:
    Adapter* m_adapter{ nullptr };
};

} // namespace vkt
