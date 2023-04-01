#pragma once

#include "gpu/swapchain.h"
#include "vulkan_api.h"

#include <memory>
#include <vector>

namespace vkt
{

class VulkanDevice;

class VulkanSwapChain : public SwapChain
{
public:
    VulkanSwapChain(VulkanDevice* device, const SwapChainDescriptor& descriptor);
    ~VulkanSwapChain() override;

    VulkanSwapChain(const SwapChain&) = delete;
    VulkanSwapChain& operator=(const SwapChain&) = delete;

    VkSwapchainKHR getVkSwapchainKHR() const;

    VkFormat getFormat() const;
    VkExtent2D getExtent2D() const;

private:
    VkSwapchainKHR m_swapchain;

    VkFormat m_format;   // TODO: need? or get from surface.
    VkExtent2D m_extent; // TODO: need? or get from surface.
};
} // namespace vkt
