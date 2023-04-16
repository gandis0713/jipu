#include "vulkan_swapchain.h"
#include "vulkan_device.h"
#include "vulkan_surface.h"
#include "vulkan_texture.h"
#include "vulkan_texture_view.h"

#include <stdexcept>

namespace vkt
{

static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableSurfaceFormats)
{
    for (const VkSurfaceFormatKHR& availableSurfaceFormat : availableSurfaceFormats)
    {
        if (availableSurfaceFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableSurfaceFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableSurfaceFormat;
        }
    }

    return availableSurfaceFormats[0];
}

static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

static VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& surfaceCapabilities)
{
    VkExtent2D extent;
    if (surfaceCapabilities.currentExtent.width != UINT32_MAX)
    {
        extent = surfaceCapabilities.currentExtent;
    }
    else
    {
        // TODO: calculate by frame buffer.
        // int frameBufferWidth, frameBufferHeight;
        // m_window->getFrameBufferSize(&frameBufferWidth, &frameBufferHeight);

        // VkExtent2D actualImageExtent = { static_cast<uint32_t>(frameBufferWidth),
        // static_cast<uint32_t>(frameBufferHeight) };

        // actualImageExtent.width = std::clamp(actualImageExtent.width, surfaceCapabilities.minImageExtent.width,
        // surfaceCapabilities.maxImageExtent.width); actualImageExtent.height = std::clamp(actualImageExtent.height,
        // surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height);

        // extent = actualImageExtent;
    }

    return extent;
}

VulkanSwapChain::VulkanSwapChain(VulkanDevice* vulkanDevice, const SwapChainDescriptor& descriptor)
    : SwapChain(vulkanDevice, descriptor)
{
    VulkanSurface* surface = static_cast<VulkanSurface*>(m_surface);

    VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(surface->getSurfaceFormats());
    VkPresentModeKHR presentMode = chooseSwapPresentMode(surface->getPresentModes());

    const VkSurfaceCapabilitiesKHR& surfaceCapabilities = surface->getSurfaceCapabilities();
    VkExtent2D imageExtent = chooseSwapExtent(surfaceCapabilities);

    uint32_t imageCount = surfaceCapabilities.minImageCount + 1;
    if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
    {
        imageCount = surfaceCapabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapchainCreateInfo{};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = surface->getSurface();
    swapchainCreateInfo.minImageCount = imageCount;
    swapchainCreateInfo.imageFormat = surfaceFormat.format;
    swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainCreateInfo.imageExtent = imageExtent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // TODO: check queue family.
    // QueueFamilyIndices foundQueueFamilyIndices = QueueFamilyIndices::findQueueFamilies(m_context.physicalDevice);
    // uint32_t queueFamilyIndices[] = { foundQueueFamilyIndices.graphicsFamily.value(),
    // foundQueueFamilyIndices.presentFamily.value() };

    // if (foundQueueFamilyIndices.graphicsFamily != foundQueueFamilyIndices.presentFamily)
    // {
    //     swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    //     swapchainCreateInfo.queueFamilyIndexCount = 2;
    //     swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;
    // }
    // else
    // {
    //     swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    //     swapchainCreateInfo.queueFamilyIndexCount = 0;     // Optional
    //     swapchainCreateInfo.pQueueFamilyIndices = nullptr; // Optional
    // }

    swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCreateInfo.queueFamilyIndexCount = 0;     // Optional
    swapchainCreateInfo.pQueueFamilyIndices = nullptr; // Optional

    swapchainCreateInfo.preTransform = surfaceCapabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = presentMode;
    swapchainCreateInfo.clipped = VK_TRUE;

    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    VkDevice device = static_cast<VulkanDevice*>(m_device)->getDevice();
    if (vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &m_swapchain) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create swap chain!");
    }

    // set format and extent.
    m_format = surfaceFormat.format;
    m_extent = imageExtent;

    // get or create swapchain image.
    vkGetSwapchainImagesKHR(device, m_swapchain, &imageCount, nullptr);

    std::vector<VkImage> images{};
    images.resize(imageCount);
    vkGetSwapchainImagesKHR(device, m_swapchain, &imageCount, images.data());

    // create Textures by VkImage.
    for (VkImage image : images)
    {
        TextureDescriptor descriptor{ vkImageType2TextureType(VK_IMAGE_TYPE_2D), vkFormat2TextureFormat(m_format) };
        std::unique_ptr<Texture> texture = std::make_unique<VulkanTexture>(vulkanDevice, image, descriptor);
        m_textures.push_back(std::move(texture));
    }

    // create TextureViews
    for (std::unique_ptr<Texture>& texture : m_textures)
    {
        TextureViewDescriptor descriptor{};
        auto textureView = std::make_unique<VulkanTextureView>(static_cast<VulkanTexture*>(texture.get()), descriptor);
        m_textureViews.push_back(std::move(textureView));
    }

    // create image views.
    // m_imageViews.resize(m_images.size());

    // for (size_t i = 0; i < m_images.size(); i++)
    // {
    //     VkImageViewCreateInfo imageViewCreateInfo{};
    //     imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    //     imageViewCreateInfo.image = m_images[i];
    //     imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    //     imageViewCreateInfo.format = m_format;

    //     imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    //     imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    //     imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    //     imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    //     imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    //     imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    //     imageViewCreateInfo.subresourceRange.levelCount = 1;
    //     imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    //     imageViewCreateInfo.subresourceRange.layerCount = 1;

    //     if (vkCreateImageView(device, &imageViewCreateInfo, nullptr, &m_imageViews[i]) != VK_SUCCESS)
    //     {
    //         throw std::runtime_error("failed to create image views!");
    //     }
    // }
}

VulkanSwapChain::~VulkanSwapChain()
{
    VkDevice device = static_cast<VulkanDevice*>(m_device)->getDevice();

    vkDestroySwapchainKHR(device, m_swapchain, nullptr);
}

VkSwapchainKHR VulkanSwapChain::getVkSwapchainKHR() const
{
    return m_swapchain;
}

VkFormat VulkanSwapChain::getFormat() const
{
    return m_format;
}

VkExtent2D VulkanSwapChain::getExtent2D() const
{
    return m_extent;
}

} // namespace vkt
