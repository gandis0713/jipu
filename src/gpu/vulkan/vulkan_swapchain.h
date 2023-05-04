#pragma once

#include "vkt/gpu/swapchain.h"
#include "vulkan_api.h"

#include "utils/cast.h"

#include <memory>
#include <vector>

namespace vkt
{

class VulkanDevice;

class VulkanSwapChain : public SwapChain
{
public:
    VulkanSwapChain(VulkanDevice* device, const SwapChainDescriptor& descriptor) noexcept(false);
    ~VulkanSwapChain() override;

    VulkanSwapChain(const SwapChain&) = delete;
    VulkanSwapChain& operator=(const SwapChain&) = delete;

    VkSwapchainKHR getVkSwapchainKHR() const;

private:
    VkSwapchainKHR m_swapchain{ VK_NULL_HANDLE };
};

DOWN_CAST(VulkanSwapChain, SwapChain);

// Convert Helper
ColorSpace VkColorSpaceKHR2ColorSpace(VkColorSpaceKHR colorSpace);
VkColorSpaceKHR ColorSpace2VkColorSpaceKHR(ColorSpace colorSpace);
PresentMode VkPresentModeKHR2PresentMode(VkPresentModeKHR mode);
VkPresentModeKHR PresentMode2VkPresentModeKHR(PresentMode mode);

} // namespace vkt
