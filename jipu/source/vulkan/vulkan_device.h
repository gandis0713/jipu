#pragma once

#include "jipu/device.h"
#include "utils/cast.h"
#include "vulkan_api.h"
#include "vulkan_binding_group_layout.h"
#include "vulkan_command_buffer.h"
#include "vulkan_export.h"
#include "vulkan_framebuffer.h"
#include "vulkan_pipeline.h"
#include "vulkan_pipeline_layout.h"
#include "vulkan_render_pass.h"
#include "vulkan_resource_allocator.h"
#include "vulkan_swapchain.h"
#include "vulkan_texture.h"

#include <memory>
#include <unordered_set>
#include <vector>

namespace jipu
{

class VulkanPhysicalDevice;
class VULKAN_EXPORT VulkanDevice : public Device
{
public:
    VulkanDevice() = delete;
    VulkanDevice(VulkanPhysicalDevice& physicalDevice, const DeviceDescriptor& descriptor);
    ~VulkanDevice() override;

    VulkanDevice(const VulkanDevice&) = delete;
    VulkanDevice& operator=(const VulkanDevice&) = delete;

public:
    std::unique_ptr<Buffer> createBuffer(const BufferDescriptor& descriptor) override;
    std::unique_ptr<BindingGroup> createBindingGroup(const BindingGroupDescriptor& descriptor) override;                   // TODO: get from cache or create.
    std::unique_ptr<BindingGroupLayout> createBindingGroupLayout(const BindingGroupLayoutDescriptor& descriptor) override; // TODO: get from cache or create.
    std::unique_ptr<CommandBuffer> createCommandBuffer(const CommandBufferDescriptor& descriptor) override;
    std::unique_ptr<PipelineLayout> createPipelineLayout(const PipelineLayoutDescriptor& descriptor) override; // TODO: get from cache or create.
    std::unique_ptr<QuerySet> createQuerySet(const QuerySetDescriptor& descriptor) override;
    std::unique_ptr<Queue> createQueue(const QueueDescriptor& descriptor) override;
    std::unique_ptr<ComputePipeline> createComputePipeline(const ComputePipelineDescriptor& descriptor) override; // TODO: get from cache or create.
    std::unique_ptr<RenderPipeline> createRenderPipeline(const RenderPipelineDescriptor& descriptor) override;    // TODO: get from cache or create.
    std::unique_ptr<Sampler> createSampler(const SamplerDescriptor& descriptor) override;
    std::unique_ptr<ShaderModule> createShaderModule(const ShaderModuleDescriptor& descriptor) override; // TODO: get from cache or create.
    std::unique_ptr<Swapchain> createSwapchain(const SwapchainDescriptor& descriptor) override;
    std::unique_ptr<Texture> createTexture(const TextureDescriptor& descriptor) override;

public:
    std::unique_ptr<RenderPipeline> createRenderPipeline(const VulkanRenderPipelineDescriptor& descriptor);
    std::unique_ptr<BindingGroupLayout> createBindingGroupLayout(const VulkanBindingGroupLayoutDescriptor& descriptor);
    std::unique_ptr<Texture> createTexture(const VulkanTextureDescriptor& descriptor);
    std::unique_ptr<Swapchain> createSwapchain(const VulkanSwapchainDescriptor& descriptor);

public:
    VulkanRenderPass& getRenderPass(const VulkanRenderPassDescriptor& descriptor);
    VulkanFramebuffer& getFrameBuffer(const VulkanFramebufferDescriptor& descriptor);
    VulkanResourceAllocator& getResourceAllocator();

public:
    VulkanPhysicalDevice& getPhysicalDevice() const;

public:
    VkDevice getVkDevice() const;
    VkPhysicalDevice getVkPhysicalDevice() const;

    VkQueue getVkQueue(uint32_t index = 0) const;

    VkCommandPool getVkCommandPool();
    VkDescriptorPool getVkDescriptorPool();

public:
    VulkanAPI vkAPI{};

private:
    void createDevice(const std::unordered_map<uint32_t, VkQueueFamilyProperties>& queueFamilies);
    const std::vector<const char*> getRequiredDeviceExtensions();

private:
    VulkanPhysicalDevice& m_physicalDevice;

private:
    VkDevice m_device = VK_NULL_HANDLE;
    VkCommandPool m_commandPool = VK_NULL_HANDLE;
    VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;

    std::vector<VkQueue> m_queues{};

    VulkanRenderPassCache m_renderPassCache;
    VulkanFramebufferCache m_frameBufferCache;
    std::unique_ptr<VulkanResourceAllocator> m_resourceAllocator = nullptr;
};

DOWN_CAST(VulkanDevice, Device);

} // namespace jipu
