#pragma once

#include "gpu/device.h"
#include "vulkan_api.h"
#include "vulkan_framebuffer.h"
#include <memory>

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
    std::unique_ptr<SwapChain> createSwapChain(SwapChainDescriptor&& descriptor) override;
    std::unique_ptr<RenderPass> createRenderPass(RenderPassDescriptor descriptor) override;
    std::unique_ptr<Pipeline> createPipeline(PipelineDescriptor descriptor) override;

    std::unique_ptr<VulkanFrameBuffer> createFrameBuffer(FramebufferCreateInfo descriptor);

public: // vulkan object
    VkDevice getDevice() const;

    VkQueue getGraphicsQueue() const;
    VkQueue getPresentQueue() const;

private:
    VkDevice m_device{};

    VkQueue m_graphicsQueue{};
    VkQueue m_presentQueue{};
};
} // namespace vkt
