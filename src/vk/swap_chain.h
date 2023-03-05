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
public:
    explicit SwapChain(const SwapChainCreateInfo info) noexcept(false);
    ~SwapChain() noexcept;

    SwapChain(const SwapChain&) = delete;
    SwapChain& operator=(const SwapChain&) = delete;
    
    void* getHandle() const;

    VkFormat getFormat() const;
    VkExtent2D getExtent2D() const;
    
    const std::vector<VkImage>& getImages() const;
    const std::vector<VkImageView>& getImageViews() const;

private:
    VkDevice m_device;
    std::weak_ptr<Surface> m_surface;

private:
    VkSwapchainKHR m_handle;
    
    std::vector<VkImage> m_images;
    std::vector<VkImageView> m_imageViews;

    VkFormat m_format; // TODO: need? or get from surface.
    VkExtent2D m_extent;    // TODO: need? or get from surface.
};
} // namespace vkt
