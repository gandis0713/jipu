#pragma once

#include "export.h"

#include "jipu/swapchain.h"
#include "vulkan_api.h"

#include "utils/cast.h"

#include <memory>
#include <vector>

namespace jipu
{

class VulkanDevice;
class JIPU_EXPERIMENTAL_EXPORT VulkanSwapchain : public Swapchain
{
public:
    VulkanSwapchain() = delete;
    VulkanSwapchain(VulkanDevice* device, const SwapchainDescriptor& descriptor) noexcept(false);
    ~VulkanSwapchain() override;

    VulkanSwapchain(const Swapchain&) = delete;
    VulkanSwapchain& operator=(const Swapchain&) = delete;

    void present(Queue* queue) override;
    int acquireNextTexture() override;

public:
    VkSwapchainKHR getVkSwapchainKHR() const;

    std::pair<VkSemaphore, VkPipelineStageFlags> getPresentSemaphore() const;
    std::pair<VkSemaphore, VkPipelineStageFlags> getRenderSemaphore() const;

private:
    VkCompositeAlphaFlagBitsKHR getCompositeAlphaFlagBit(VkCompositeAlphaFlagsKHR supportedCompositeAlpha);

private:
    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    VkSemaphore m_presentSemaphore = VK_NULL_HANDLE;
    VkSemaphore m_renderSemaphore = VK_NULL_HANDLE;
    uint32_t m_acquiredImageIndex = 0u;
};

DOWN_CAST(VulkanSwapchain, Swapchain);

// Convert Helper
ColorSpace ToColorSpace(VkColorSpaceKHR colorSpace);
VkColorSpaceKHR ToVkColorSpaceKHR(ColorSpace colorSpace);
PresentMode ToPresentMode(VkPresentModeKHR mode);
VkPresentModeKHR ToVkPresentModeKHR(PresentMode mode);

} // namespace jipu
