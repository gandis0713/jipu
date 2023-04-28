#pragma once

#include "gpu/device.h"
#include "vulkan_api.h"
#include "vulkan_framebuffer.h"
#include "vulkan_render_pass.h"

#include <memory>
#include <unordered_set>
#include <vector>

namespace vkt
{

class VulkanAdapter;

class VulkanDevice : public Device
{
public:
    VulkanDevice() = delete;
    VulkanDevice(VulkanAdapter* adapter, DeviceDescriptor descriptor);
    ~VulkanDevice() override;

    VulkanDevice(const VulkanDevice&) = delete;
    VulkanDevice& operator=(const VulkanDevice&) = delete;

public:
    std::unique_ptr<SwapChain> createSwapChain(const SwapChainDescriptor& descriptor) override;
    std::unique_ptr<Pipeline> createPipeline(const PipelineDescriptor& descriptor) override;
    std::unique_ptr<Queue> createQueue(const QueueDescriptor& descriptor) override;

    VulkanRenderPass* getRenderPass(const VulkanRenderPassDescriptor& descriptor);
    VulkanFrameBuffer* getFrameBuffer(const VulkanFramebufferDescriptor& descriptor);

public: // vulkan object
    VkDevice getDevice() const;
    VkPhysicalDevice getPhysicalDevice() const;

    VkQueue getQueue() const;
    uint32_t getQueueIndex() const;

public:
    VulkanAPI vkAPI{};

private:
    void createDevice(const std::unordered_set<uint32_t>& queueFamilyIndices);

    void gatherQueues();
    bool checkDeviceExtensionSupport(const std::vector<const char*>& requiredDeviceExtensions);

private:
    VkDevice m_device{};

    std::vector<VkQueue> m_queues{};

    VulkanRenderPassCache m_renderPassCache;
    VulkanFrameBufferCache m_frameBufferCache;
};
} // namespace vkt
