#pragma once

#include "gpu/device.h"
#include "vulkan_api.h"
#include <memory>

namespace vkt
{

class VulkanAdapter;

class VulkanDevice : public Device
{
public:
    VulkanDevice() = delete;
    VulkanDevice(VulkanAdapter* adapter, DeviceCreateInfo info);
    ~VulkanDevice() override;

    VulkanDevice(const VulkanDevice&) = delete;
    VulkanDevice& operator=(const VulkanDevice&) = delete;

public:
    std::unique_ptr<SwapChain> createSwapChain(SwapChainCreateInfo&& info) override;
    std::unique_ptr<RenderPass> createRenderPass(RenderPassCreateInfo info) override;
    std::unique_ptr<Pipeline> createPipeline(PipelineCreateInfo info) override;

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
