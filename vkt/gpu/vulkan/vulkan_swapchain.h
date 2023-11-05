#pragma once

#include "vkt/gpu/swapchain.h"
#include "vulkan_api.h"

#include "utils/cast.h"

#include <memory>
#include <vector>

namespace vkt
{

class VulkanDevice;

class VulkanSwapchain : public Swapchain
{
public:
    VulkanSwapchain() = delete;
    VulkanSwapchain(VulkanDevice* device, const SwapchainDescriptor& descriptor) noexcept(false);
    ~VulkanSwapchain() override;

    VulkanSwapchain(const Swapchain&) = delete;
    VulkanSwapchain& operator=(const Swapchain&) = delete;

    void present(Queue* queue) override;
    int acquireNextTexture() override;
    TextureView* getTextureView(uint32_t index) override;

public:
    VkSwapchainKHR getVkSwapchainKHR() const;

    void injectSignalSemaphore(VkSemaphore semaphore);
    std::pair<VkSemaphore, VkPipelineStageFlags> getSignalSemaphore() const;

private:
    VkCompositeAlphaFlagBitsKHR getCompositeAlphaFlagBit(VkCompositeAlphaFlagsKHR supportedCompositeAlpha);

private:
    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    VkSemaphore m_presentSemaphore = VK_NULL_HANDLE;
    uint32_t m_acquiredImageIndex = 0u;

    std::vector<VkSemaphore> m_waitSemaphores = {};
};

DOWN_CAST(VulkanSwapchain, Swapchain);

// Convert Helper
ColorSpace ToColorSpace(VkColorSpaceKHR colorSpace);
VkColorSpaceKHR ToVkColorSpaceKHR(ColorSpace colorSpace);
PresentMode ToPresentMode(VkPresentModeKHR mode);
VkPresentModeKHR ToVkPresentModeKHR(PresentMode mode);

} // namespace vkt
