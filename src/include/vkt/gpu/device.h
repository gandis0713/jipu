#pragma once

#include "export.h"
#include "vkt/gpu/pipeline.h"
#include "vkt/gpu/queue.h"
#include "vkt/gpu/swapchain.h"

#include <memory>

namespace vkt
{

class Adapter;

struct DeviceDescriptor
{
};

class VKT_EXPORT Device
{
public:
    Device() = delete;
    Device(Adapter* adapter, DeviceDescriptor descriptor);
    virtual ~Device() = default;

    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;

    Adapter* getAdapter() const;

    virtual std::unique_ptr<SwapChain> createSwapChain(const SwapChainDescriptor& descriptor) = 0;
    virtual std::unique_ptr<Pipeline> createPipeline(const PipelineDescriptor& descriptor) = 0;
    virtual std::unique_ptr<Queue> createQueue(const QueueDescriptor& descriptor) = 0;

protected:
    Adapter* m_adapter{ nullptr };
};

} // namespace vkt
