#include "vulkan_swapchain.h"
#include "vulkan_device.h"
#include "vulkan_surface.h"
#include "vulkan_texture.h"
#include "vulkan_texture_view.h"

#include "utils/log.h"

#include <fmt/format.h>
#include <stdexcept>

namespace vkt
{

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

VulkanSwapChain::VulkanSwapChain(VulkanDevice* vulkanDevice, const SwapChainDescriptor& descriptor) noexcept(false)
    : SwapChain(vulkanDevice, descriptor)
{
    VulkanSurface* surface = downcast(m_surface);

    const VulkanSurfaceInfo& surfaceInfo = surface->gatherSurfaceInfo(vulkanDevice->getVkPhysicalDevice());

    // Check surface formats supports.
    auto surfaceFormatIter = std::find_if(surfaceInfo.formats.begin(),
                                          surfaceInfo.formats.end(),
                                          [textureFormat = m_textureFormat, colorSpace = m_colorSpace](const VkSurfaceFormatKHR& surfaceFormat)
                                          { return surfaceFormat.format == TextureFormat2VkFormat(textureFormat) &&
                                                   surfaceFormat.colorSpace == ColorSpace2VkColorSpaceKHR(colorSpace); });
    if (surfaceFormatIter == surfaceInfo.formats.end())
    {
        throw std::runtime_error(fmt::format("{} texture format or/and {} color space are not supported.",
                                             static_cast<uint32_t>(m_textureFormat),
                                             static_cast<uint32_t>(m_colorSpace)));
    }
    const VkSurfaceFormatKHR surfaceFormat = *surfaceFormatIter;

    // Check surface present mode.
    auto presentModeIter = std::find(surfaceInfo.presentModes.begin(),
                                     surfaceInfo.presentModes.end(),
                                     PresentMode2VkPresentModeKHR(m_presentMode));
    if (presentModeIter == surfaceInfo.presentModes.end())
    {
        throw std::runtime_error(fmt::format("{} present mode is not supported.", static_cast<uint32_t>(m_presentMode)));
    }
    const VkPresentModeKHR presentMode = *presentModeIter;

    // Check surface capabilities.
    const VkSurfaceCapabilitiesKHR& surfaceCapabilities = surfaceInfo.capabilities;
    uint32_t imageCount = surfaceCapabilities.minImageCount + 1;
    if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
    {
        imageCount = surfaceCapabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapchainCreateInfo{};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = surface->getSurfaceKHR();
    swapchainCreateInfo.minImageCount = imageCount;
    swapchainCreateInfo.imageFormat = surfaceFormat.format;
    swapchainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;
    swapchainCreateInfo.imageExtent = { m_width, m_height };
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // TODO: check sharing mode
    // if graphics and present queue family are difference.
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

    VkDevice device = downcast(m_device)->getVkDevice();
    const VulkanAPI& vkAPI = downcast(m_device)->vkAPI;
    if (vkAPI.CreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &m_swapchain) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create swap chain!");
    }

    // get swapchain image.
    vkAPI.GetSwapchainImagesKHR(device, m_swapchain, &imageCount, nullptr);

    std::vector<VkImage> images{};
    images.resize(imageCount);
    vkAPI.GetSwapchainImagesKHR(device, m_swapchain, &imageCount, images.data());

    // create Textures by VkImage.
    for (VkImage image : images)
    {
        TextureDescriptor descriptor{ TextureType::k2D, m_textureFormat };
        std::unique_ptr<Texture> texture = std::make_unique<VulkanTexture>(vulkanDevice, image, descriptor);
        m_textures.push_back(std::move(texture));
    }

    // create TextureViews
    for (std::unique_ptr<Texture>& texture : m_textures)
    {
        TextureViewDescriptor descriptor{};
        auto textureView = std::make_unique<VulkanTextureView>(downcast(texture.get()), descriptor);
        m_textureViews.push_back(std::move(textureView));
    }
}

VulkanSwapChain::~VulkanSwapChain()
{
    const VulkanAPI& vkAPI = downcast(m_device)->vkAPI;
    vkAPI.DestroySwapchainKHR(downcast(m_device)->getVkDevice(), m_swapchain, nullptr);
}

VkSwapchainKHR VulkanSwapChain::getVkSwapchainKHR() const
{
    return m_swapchain;
}

ColorSpace VkColorSpaceKHR2ColorSpace(VkColorSpaceKHR colorSpace)
{
    switch (colorSpace)
    {
    case VK_COLOR_SPACE_SRGB_NONLINEAR_KHR:
        return ColorSpace::kSRGBNonLinear;

    case VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT:
        return ColorSpace::kSRGBLinear;

    default:
        return ColorSpace::kUndefined;
    }
}
VkColorSpaceKHR ColorSpace2VkColorSpaceKHR(ColorSpace colorSpace)
{
    switch (colorSpace)
    {
    case ColorSpace::kSRGBNonLinear:
        return VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

    case ColorSpace::kSRGBLinear:
        return VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT;

    case ColorSpace::kUndefined:
    default:
        LOG_ERROR("color space is undefined. use srgb non linear mode.");
        return VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    }
}

PresentMode VkPresentModeKHR2PresentMode(VkPresentModeKHR mode)
{
    switch (mode)
    {
    case VK_PRESENT_MODE_MAILBOX_KHR:
        return PresentMode::kMailbox;
    case VK_PRESENT_MODE_FIFO_KHR:
        return PresentMode::kFifo;
    case VK_PRESENT_MODE_IMMEDIATE_KHR:
        return PresentMode::kImmediate;
    default:
        return PresentMode::kUndefined;
    }
}
VkPresentModeKHR PresentMode2VkPresentModeKHR(PresentMode mode)
{
    switch (mode)
    {
    case PresentMode::kMailbox:
        return VK_PRESENT_MODE_MAILBOX_KHR;
    case PresentMode::kFifo:
        return VK_PRESENT_MODE_FIFO_KHR;
    case PresentMode::kImmediate:
        return VK_PRESENT_MODE_IMMEDIATE_KHR;
    default:
        LOG_ERROR("Present Mode is undefined. use immediate mode.");
        return VK_PRESENT_MODE_IMMEDIATE_KHR;
    }
}

} // namespace vkt
