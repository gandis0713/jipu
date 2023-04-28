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

private:
    VkSwapchainKHR m_swapchain{ VK_NULL_HANDLE };
};

// Convert Helper
PresentMode VkPresentModeKHR2PresentMode(VkPresentModeKHR mode);
VkPresentModeKHR PresentMode2VkPresentModeKHR(PresentMode mode);

} // namespace vkt
