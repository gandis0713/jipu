#pragma once

#include "utils/cast.h"
#include "vkt/gpu/device.h"
#include "vulkan_api.h"
#include "vulkan_framebuffer.h"
#include "vulkan_render_pass.h"

#include <memory>
#include <unordered_set>
#include <vector>

namespace vkt
{

class VulkanPhysicalDevice;

class VKT_EXPORT VulkanDevice : public Device
{
public:
    VulkanDevice() = delete;
    VulkanDevice(VulkanPhysicalDevice* physicalDevice, DeviceDescriptor descriptor);
    ~VulkanDevice() override;

    VulkanDevice(const VulkanDevice&) = delete;
    VulkanDevice& operator=(const VulkanDevice&) = delete;

public:
    std::unique_ptr<SwapChain> createSwapChain(const SwapChainDescriptor& descriptor) override;
    std::unique_ptr<Pipeline> createPipeline(const PipelineDescriptor& descriptor) override;
    std::unique_ptr<Queue> createQueue(const QueueDescriptor& descriptor) override;
    std::unique_ptr<Buffer> createBuffer(const BufferDescriptor& descriptor) override;

    VulkanRenderPass* getRenderPass(const VulkanRenderPassDescriptor& descriptor);
    VulkanFrameBuffer* getFrameBuffer(const VulkanFramebufferDescriptor& descriptor);

public:
    VkDevice getVkDevice() const;
    VkPhysicalDevice getVkPhysicalDevice() const;

    VkQueue getQueue() const;
    uint32_t getQueueIndex() const;

public:
    VulkanAPI vkAPI{};

private:
    void createDevice(const std::unordered_set<uint32_t>& queueFamilyIndices);

    void gatherQueues();
    bool checkDeviceExtensionSupport(const std::vector<const char*>& requiredDeviceExtensions);

private:
    VkDevice m_device{ VK_NULL_HANDLE };

    std::vector<VkQueue> m_queues{};

    VulkanRenderPassCache m_renderPassCache;
    VulkanFrameBufferCache m_frameBufferCache;
};

DOWN_CAST(VulkanDevice, Device);

} // namespace vkt