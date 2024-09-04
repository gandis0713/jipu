#pragma once

#include "export.h"
#include "jipu/binding_group.h"
#include "jipu/binding_group_layout.h"
#include "jipu/buffer.h"
#include "jipu/command_buffer.h"
#include "jipu/pipeline.h"
#include "jipu/pipeline_layout.h"
#include "jipu/query_set.h"
#include "jipu/queue.h"
#include "jipu/sampler.h"
#include "jipu/shader_module.h"
#include "jipu/swapchain.h"
#include "jipu/texture.h"

#include <memory>

namespace jipu
{

struct DeviceDescriptor
{
};

class JIPU_EXPORT Device
{
public:
    virtual ~Device() = default;

    Device(const Device&) = delete;
    Device& operator=(const Device&) = delete;

protected:
    Device() = default;

public:
    virtual std::unique_ptr<Buffer> createBuffer(const BufferDescriptor& descriptor) = 0;
    virtual std::unique_ptr<BindingGroup> createBindingGroup(const BindingGroupDescriptor& descriptor) = 0;
    virtual std::unique_ptr<BindingGroupLayout> createBindingGroupLayout(const BindingGroupLayoutDescriptor& descriptor) = 0;
    virtual std::unique_ptr<CommandBuffer> createCommandBuffer(const CommandBufferDescriptor& descriptor) = 0;
    virtual std::unique_ptr<PipelineLayout> createPipelineLayout(const PipelineLayoutDescriptor& descriptor) = 0;
    virtual std::unique_ptr<QuerySet> createQuerySet(const QuerySetDescriptor& descriptor) = 0;
    virtual std::unique_ptr<Queue> createQueue(const QueueDescriptor& descriptor) = 0;
    virtual std::unique_ptr<ComputePipeline> createComputePipeline(const ComputePipelineDescriptor& descriptor) = 0;
    virtual std::unique_ptr<RenderPipeline> createRenderPipeline(const RenderPipelineDescriptor& descriptor) = 0;
    virtual std::unique_ptr<Sampler> createSampler(const SamplerDescriptor& descriptor) = 0;
    virtual std::unique_ptr<ShaderModule> createShaderModule(const ShaderModuleDescriptor& descriptor) = 0;
    virtual std::unique_ptr<Swapchain> createSwapchain(const SwapchainDescriptor& descriptor) = 0;
    virtual std::unique_ptr<Texture> createTexture(const TextureDescriptor& descriptor) = 0;
};

} // namespace jipu
