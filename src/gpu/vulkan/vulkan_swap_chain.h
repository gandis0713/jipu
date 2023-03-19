#pragma once

#include "gpu/swap_chain.h"
#include "vulkan_api.h"

#include <memory>
#include <vector>

namespace vkt
{

class VulkanDevice;

class VulkanSwapChain : public SwapChain
{
public:
    VulkanSwapChain(VulkanDevice* device, SwapChainCreateInfo info);
    ~VulkanSwapChain() override;

    VulkanSwapChain(const SwapChain&) = delete;
    VulkanSwapChain& operator=(const SwapChain&) = delete;

    VkSwapchainKHR getVkSwapchainKHR() const;

    VkFormat getFormat() const;
    VkExtent2D getExtent2D() const;

    const std::vector<VkImage>& getImages() const;
    const std::vector<VkImageView>& getImageViews() const;

private:
    VkSwapchainKHR m_swapchain;

    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;

    VkFormat m_format;   // TODO: need? or get from surface.
    VkExtent2D m_extent; // TODO: need? or get from surface.
};
} // namespace vkt
