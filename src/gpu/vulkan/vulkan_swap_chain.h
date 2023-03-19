#pragma once

#include "gpu/swap_chain.h"
#include "vulkan_api.h"
#include "vulkan_surface.h"

#include <memory>
#include <vector>

namespace vkt
{

struct SwapChainCreateHandles
{
    VkDevice device{ nullptr };
    std::unique_ptr<VulkanSurface> surface{ nullptr };
};

class VulkanSwapChain : public SwapChain
{
public:
    VulkanSwapChain(SwapChainCreateHandles handles, SwapChainCreateInfo info) noexcept;
    ~VulkanSwapChain() override;

    VulkanSwapChain(const SwapChain&) = delete;
    VulkanSwapChain& operator=(const SwapChain&) = delete;

    void* getHandle() const;

    VkFormat getFormat() const;
    VkExtent2D getExtent2D() const;

    const std::vector<VkImage>& getImages() const;
    const std::vector<VkImageView>& getImageViews() const;

private:
    VkDevice m_device;
    std::unique_ptr<VulkanSurface> m_surface{ nullptr };

private:
    VkSwapchainKHR m_handle;

    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;

    VkFormat m_format;   // TODO: need? or get from surface.
    VkExtent2D m_extent; // TODO: need? or get from surface.
};
} // namespace vkt
