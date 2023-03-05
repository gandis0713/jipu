#pragma once

#include "surface.h"
#include "vulkan_api.h"

#include <vector>

namespace vkt
{

struct SwapChainCreateInfo
{
    VkDevice device;
    std::weak_ptr<Surface> surface;
};

class SwapChain
{
    explicit SwapChain(const SwapChainCreateInfo info);
    ~SwapChain();

    SwapChain(const SwapChain&) = delete;
    SwapChain& operator=(const SwapChain&) = delete;

private:
    VkDevice m_device;
    std::weak_ptr<Surface> m_surface;

private:
    std::vector<VkImage> m_vecSwapChainImages;
    VkSwapchainKHR m_swapChain;
    std::vector<VkImageView> m_vecSwapChainImageViews;
    std::vector<VkFramebuffer> m_vecSwapChainFramebuffers;

    VkFormat m_swapChainImageFormat; // TODO: need? or get from surface.
    VkExtent2D m_swapChainExtent;    // TODO: need? or get from surface.
};
} // namespace vkt