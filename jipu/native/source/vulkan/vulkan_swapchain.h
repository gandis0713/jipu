#pragma once

#include "jipu/swapchain.h"

#include "vulkan_api.h"
#include "vulkan_export.h"
#include "vulkan_texture.h"
#include "vulkan_texture_view.h"

#include "common/cast.h"

#include <memory>
#include <vector>

namespace jipu
{

class VulkanDevice;
class VulkanTexture;
class VulkanSurface;
class VulkanTextureView;

struct VulkanSwapchainDescriptor
{
    const void* next = nullptr;
    VkSwapchainCreateFlagsKHR flags = 0u;
    VulkanSurface& surface;
    uint32_t minImageCount = 0;
    VkFormat imageFormat;
    VkColorSpaceKHR imageColorSpace;
    VkExtent2D imageExtent;
    uint32_t imageArrayLayers;
    VkImageUsageFlags imageUsage;
    VkSharingMode imageSharingMode;
    std::vector<uint32_t> queueFamilyIndices{};
    VkSurfaceTransformFlagBitsKHR preTransform;
    VkCompositeAlphaFlagBitsKHR compositeAlpha;
    VkPresentModeKHR presentMode;
    VkBool32 clipped;
    VkSwapchainKHR oldSwapchain;
};

class VULKAN_EXPORT VulkanSwapchain : public Swapchain
{
public:
    VulkanSwapchain() = delete;
    VulkanSwapchain(VulkanDevice& device, const SwapchainDescriptor& descriptor) noexcept(false);
    VulkanSwapchain(VulkanDevice& device, const VulkanSwapchainDescriptor& descriptor) noexcept(false);
    ~VulkanSwapchain() override;

    VulkanSwapchain(const Swapchain&) = delete;
    VulkanSwapchain& operator=(const Swapchain&) = delete;

    TextureFormat getTextureFormat() const override;
    uint32_t getWidth() const override;
    uint32_t getHeight() const override;

    void present(Queue& queue) override;
    TextureView& acquireNextTexture() override;

public:
    VkSwapchainKHR getVkSwapchainKHR() const;

    std::pair<VkSemaphore, VkPipelineStageFlags> getPresentSemaphore() const;

private:
    VulkanDevice& m_device;
    const VulkanSwapchainDescriptor m_descriptor;

    std::vector<std::unique_ptr<VulkanTexture>> m_textures{};
    std::vector<std::unique_ptr<VulkanTextureView>> m_textureViews{};

private:
    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    VkSemaphore m_presentSemaphore = VK_NULL_HANDLE;
    uint32_t m_acquiredImageIndex = 0u;
};

DOWN_CAST(VulkanSwapchain, Swapchain);

// Generate Helper
VulkanSwapchainDescriptor VULKAN_EXPORT generateVulkanSwapchainDescriptor(VulkanDevice& device, const SwapchainDescriptor& descriptor);

// Convert Helper
ColorSpace ToColorSpace(VkColorSpaceKHR colorSpace);
VkColorSpaceKHR ToVkColorSpaceKHR(ColorSpace colorSpace);
PresentMode ToPresentMode(VkPresentModeKHR mode);
VkPresentModeKHR ToVkPresentModeKHR(PresentMode mode);

} // namespace jipu
