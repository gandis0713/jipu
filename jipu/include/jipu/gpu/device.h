#pragma once

#include "export.h"
#include "jipu/gpu/binding_group.h"
#include "jipu/gpu/binding_group_layout.h"
#include "jipu/gpu/buffer.h"
#include "jipu/gpu/command_buffer.h"
#include "jipu/gpu/pipeline.h"
#include "jipu/gpu/pipeline_layout.h"
#include "jipu/gpu/queue.h"
#include "jipu/gpu/sampler.h"
#include "jipu/gpu/shader_module.h"
#include "jipu/gpu/swapchain.h"
#include "jipu/gpu/texture.h"

#include <memory>

namespace jipu
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

public:
    virtual std::unique_ptr<Buffer> createBuffer(const BufferDescriptor& descriptor) = 0;
    virtual std::unique_ptr<BindingGroup> createBindingGroup(const BindingGroupDescriptor& descriptor) = 0;
    virtual std::unique_ptr<BindingGroupLayout> createBindingGroupLayout(const BindingGroupLayoutDescriptor& descriptor) = 0;
    virtual std::unique_ptr<CommandBuffer> createCommandBuffer(const CommandBufferDescriptor& descriptor) = 0;
    virtual std::unique_ptr<PipelineLayout> createPipelineLayout(const PipelineLayoutDescriptor& descriptor) = 0;
    virtual std::unique_ptr<Queue> createQueue(const QueueDescriptor& descriptor) = 0;
    virtual std::unique_ptr<ComputePipeline> createComputePipeline(const ComputePipelineDescriptor& descriptor) = 0;
    virtual std::unique_ptr<RenderPipeline> createRenderPipeline(const RenderPipelineDescriptor& descriptor) = 0;
    virtual std::unique_ptr<Sampler> createSampler(const SamplerDescriptor& descriptor) = 0;
    virtual std::unique_ptr<ShaderModule> createShaderModule(const ShaderModuleDescriptor& descriptor) = 0;
    virtual std::unique_ptr<Swapchain> createSwapchain(const SwapchainDescriptor& descriptor) = 0;
    virtual std::unique_ptr<Texture> createTexture(const TextureDescriptor& descriptor) = 0;

public:
    PhysicalDevice* getPhysicalDevice() const;

protected:
    PhysicalDevice* m_physicalDevice = nullptr;
};

} // namespace jipu
