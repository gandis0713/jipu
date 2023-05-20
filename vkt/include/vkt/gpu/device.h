#pragma once

#include "export.h"
#include "vkt/gpu/buffer.h"
#include "vkt/gpu/command_buffer.h"
#include "vkt/gpu/pipeline.h"
#include "vkt/gpu/queue.h"
#include "vkt/gpu/shader_module.h"
#include "vkt/gpu/swapchain.h"

#include <memory>

namespace vkt
{

class PhysicalDevice;

struct DeviceDescriptor
{
};

class VKT_EXPORT Device
{
public:
    Device() = delete;
    Device(PhysicalDevice* physicalDevice, DeviceDescriptor descriptor);
    virtual ~Device() = default;

    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;

    PhysicalDevice* getPhysicalDevice() const;

    virtual std::unique_ptr<SwapChain> createSwapChain(const SwapChainDescriptor& descriptor) = 0;
    virtual std::unique_ptr<Pipeline> createPipeline(const PipelineDescriptor& descriptor) = 0;
    virtual std::unique_ptr<Queue> createQueue(const QueueDescriptor& descriptor) = 0;
    virtual std::unique_ptr<Buffer> createBuffer(const BufferDescriptor& descriptor) = 0;
    virtual std::unique_ptr<CommandBuffer> createCommandBuffer(const CommandBufferDescriptor& descriptor) = 0;
    virtual std::unique_ptr<ShaderModule> createShaderModule(const ShaderModuleDescriptor& descriptor) = 0;

protected:
    PhysicalDevice* m_physicalDevice = nullptr;
};

} // namespace vkt
