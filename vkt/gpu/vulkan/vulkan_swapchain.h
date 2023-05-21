#pragma once

#include "vkt/gpu/swapchain.h"
#include "vulkan_api.h"

#include "utils/cast.h"

#include <memory>
#include <vector>

namespace vkt
{

class VulkanDevice;

class VKT_EXPORT VulkanSwapchain : public Swapchain
{
public:
    VulkanSwapchain(VulkanDevice* device, const SwapchainDescriptor& descriptor) noexcept(false);
    ~VulkanSwapchain() override;

    VulkanSwapchain(const Swapchain&) = delete;
    VulkanSwapchain& operator=(const Swapchain&) = delete;

    void present() override;
    TextureView* getCurrentView() override;

public:
    VkSwapchainKHR getVkSwapchainKHR() const;

private:
    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    VkSemaphore m_imageAvailableSemaphore = VK_NULL_HANDLE;
    std::vector<VkSemaphore> m_semaphores{};
};

DOWN_CAST(VulkanSwapchain, Swapchain);

// Convert Helper
ColorSpace VkColorSpaceKHR2ColorSpace(VkColorSpaceKHR colorSpace);
VkColorSpaceKHR ColorSpace2VkColorSpaceKHR(ColorSpace colorSpace);
PresentMode VkPresentModeKHR2PresentMode(VkPresentModeKHR mode);
VkPresentModeKHR PresentMode2VkPresentModeKHR(PresentMode mode);

} // namespace vkt
