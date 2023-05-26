#pragma once

#include "utils/cast.h"
#include "vkt/gpu/device.h"
#include "vulkan_api.h"
#include "vulkan_command_buffer.h"
#include "vulkan_framebuffer.h"
#include "vulkan_render_pass.h"
#include "vulkan_synchronization.h"

#include <memory>
#include <unordered_set>
#include <vector>

namespace vkt
{

class VulkanPhysicalDevice;

class VulkanDevice : public Device
{
public:
    VulkanDevice() = delete;
    VulkanDevice(VulkanPhysicalDevice* physicalDevice, DeviceDescriptor descriptor);
    ~VulkanDevice() override;

    VulkanDevice(const VulkanDevice&) = delete;
    VulkanDevice& operator=(const VulkanDevice&) = delete;

public:
    std::unique_ptr<Swapchain> createSwapchain(const SwapchainDescriptor& descriptor) override;
    std::unique_ptr<RenderPipeline> createRenderPipeline(const RenderPipelineDescriptor& descriptor) override;
    std::unique_ptr<Queue> createQueue(const QueueDescriptor& descriptor) override;
    std::unique_ptr<Buffer> createBuffer(const BufferDescriptor& descriptor) override;
    std::unique_ptr<CommandBuffer> createCommandBuffer(const CommandBufferDescriptor& descriptor) override;
    std::unique_ptr<ShaderModule> createShaderModule(const ShaderModuleDescriptor& descriptor) override;

public:
    VulkanRenderPass* getRenderPass(const VulkanRenderPassDescriptor& descriptor);
    VulkanFrameBuffer* getFrameBuffer(const VulkanFramebufferDescriptor& descriptor);

    VulkanSynchronization& getSynchronization();

public:
    VkDevice getVkDevice() const;
    VkPhysicalDevice getVkPhysicalDevice() const;

    VkQueue getVkQueue(uint32_t index = 0) const;

    VkCommandPool getCommandPool();

public:
    VulkanAPI vkAPI{};

private:
    void createDevice(const std::unordered_set<uint32_t>& queueFamilyIndices);

private:
    VkDevice m_device = VK_NULL_HANDLE;
    VkCommandPool m_commandPool = VK_NULL_HANDLE;

    std::vector<VkQueue> m_queues{};

    std::vector<VkSemaphore> m_waitSemaphore{};
    std::vector<VkSemaphore> m_signalSemaphore{};

    VulkanRenderPassCache m_renderPassCache;
    VulkanFrameBufferCache m_frameBufferCache;
    VulkanSynchronization m_synchronization;
};

DOWN_CAST(VulkanDevice, Device);

} // namespace vkt
