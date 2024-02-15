#include "vulkan_swapchain.h"

#include "vulkan_device.h"
#include "vulkan_queue.h"
#include "vulkan_surface.h"

#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace jipu
{

VulkanSwapchain::VulkanSwapchain(VulkanDevice* device, const SwapchainDescriptor& descriptor) noexcept(false)
    : m_device(device)
    , m_descriptor(descriptor)
{
    VulkanSurface* surface = downcast(m_descriptor.surface);

    const VulkanSurfaceInfo& surfaceInfo = surface->gatherSurfaceInfo(m_device->getVkPhysicalDevice());

    // Check surface formats supports.
    auto surfaceFormatIter = std::find_if(surfaceInfo.formats.begin(),
                                          surfaceInfo.formats.end(),
                                          [textureFormat = m_descriptor.textureFormat, colorSpace = m_descriptor.colorSpace](const VkSurfaceFormatKHR& surfaceFormat) { return surfaceFormat.format == ToVkFormat(textureFormat) &&
                                                                                                                                                                               surfaceFormat.colorSpace == ToVkColorSpaceKHR(colorSpace); });
    if (surfaceFormatIter == surfaceInfo.formats.end())
    {
        throw std::runtime_error(fmt::format("{} texture format or/and {} color space are not supported.",
                                             static_cast<uint32_t>(m_descriptor.textureFormat),
                                             static_cast<uint32_t>(m_descriptor.colorSpace)));
    }
    const VkSurfaceFormatKHR surfaceFormat = *surfaceFormatIter;

    // Check surface present mode.
    auto presentModeIter = std::find(surfaceInfo.presentModes.begin(),
                                     surfaceInfo.presentModes.end(),
                                     ToVkPresentModeKHR(m_descriptor.presentMode));
    if (presentModeIter == surfaceInfo.presentModes.end())
    {
        throw std::runtime_error(fmt::format("{} present mode is not supported.", static_cast<uint32_t>(m_descriptor.presentMode)));
    }
    const VkPresentModeKHR presentMode = *presentModeIter;

    // Check surface capabilities.
    const VkSurfaceCapabilitiesKHR& surfaceCapabilities = surfaceInfo.capabilities;
    uint32_t imageCount = surfaceCapabilities.minImageCount + 1;
    if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
    {
        imageCount = surfaceCapabilities.maxImageCount;
    }

    m_width = m_descriptor.width;
    m_height = m_descriptor.height;

    // If extent is invalid, use current extent.
    if (m_descriptor.width < surfaceCapabilities.minImageExtent.width ||
        m_descriptor.width > surfaceCapabilities.maxImageExtent.width ||
        m_descriptor.height < surfaceCapabilities.minImageExtent.height ||
        m_descriptor.height > surfaceCapabilities.maxImageExtent.height)
    {
        m_width = surfaceCapabilities.currentExtent.width;
        m_height = surfaceCapabilities.currentExtent.height;
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
    swapchainCreateInfo.compositeAlpha = getCompositeAlphaFlagBit(surfaceCapabilities.supportedCompositeAlpha);
    swapchainCreateInfo.presentMode = presentMode;
    swapchainCreateInfo.clipped = VK_TRUE;

    swapchainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

    const VulkanAPI& vkAPI = m_device->vkAPI;
    if (vkAPI.CreateSwapchainKHR(m_device->getVkDevice(), &swapchainCreateInfo, nullptr, &m_swapchain) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create swap chain");
    }

    // get swapchain image.
    vkAPI.GetSwapchainImagesKHR(m_device->getVkDevice(), m_swapchain, &imageCount, nullptr);

    std::vector<VkImage> images{};
    images.resize(imageCount);
    vkAPI.GetSwapchainImagesKHR(m_device->getVkDevice(), m_swapchain, &imageCount, images.data());

    // create Textures by VkImage.
    for (VkImage image : images)
    {
        TextureDescriptor textureDescriptor{ .type = TextureType::k2D,
                                             .format = m_descriptor.textureFormat,
                                             .usage = TextureUsageFlagBits::kColorAttachment,
                                             .width = m_width,
                                             .height = m_height,
                                             .depth = 1,
                                             .mipLevels = 1,
                                             .sampleCount = 1 };
        std::unique_ptr<VulkanTexture> texture = std::make_unique<VulkanTexture>(m_device, image, textureDescriptor);
        m_textures.push_back(std::move(texture));
    }

    // create TextureViews
    for (std::unique_ptr<VulkanTexture>& texture : m_textures)
    {
        TextureViewDescriptor descriptor{};
        descriptor.type = TextureViewType::k2D;
        descriptor.aspect = TextureAspectFlagBits::kColor;
        auto textureView = std::make_unique<VulkanTextureView>(downcast(texture.get()), descriptor);
        m_textureViews.push_back(std::move(textureView));
    }

    // create semaphore
    VkSemaphoreCreateInfo semaphoreCreateInfo{};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphoreCreateInfo.pNext = nullptr;
    semaphoreCreateInfo.flags = 0;

    if (vkAPI.CreateSemaphore(m_device->getVkDevice(), &semaphoreCreateInfo, nullptr, &m_presentSemaphore) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create semephore for present.");
    }

    if (vkAPI.CreateSemaphore(m_device->getVkDevice(), &semaphoreCreateInfo, nullptr, &m_renderSemaphore) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create semaphore for rendering.");
    }
}

VulkanSwapchain::~VulkanSwapchain()
{
    auto vulkanDevice = downcast(m_device);
    const VulkanAPI& vkAPI = vulkanDevice->vkAPI;

    vkAPI.DestroySemaphore(vulkanDevice->getVkDevice(), m_presentSemaphore, nullptr);
    vkAPI.DestroySemaphore(vulkanDevice->getVkDevice(), m_renderSemaphore, nullptr);

    /* do not delete VkImages from swapchain. */

    vkAPI.DestroySwapchainKHR(downcast(m_device)->getVkDevice(), m_swapchain, nullptr);
}

TextureFormat VulkanSwapchain::getTextureFormat() const
{
    return m_descriptor.textureFormat;
}

uint32_t VulkanSwapchain::getWidth() const
{
    return m_width;
}

uint32_t VulkanSwapchain::getHeight() const
{
    return m_height;
}

void VulkanSwapchain::present(Queue* queue)
{
    VulkanDevice* vulkanDevice = downcast(m_device);
    VulkanQueue* vulkanQueue = downcast(queue);
    const VulkanAPI& vkAPI = vulkanDevice->vkAPI;

    // present
    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &m_renderSemaphore;

    VkSwapchainKHR swapChains[] = { m_swapchain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &m_acquiredImageIndex;
    presentInfo.pResults = nullptr; // Optional

    vkAPI.QueuePresentKHR(vulkanQueue->getVkQueue(), &presentInfo);
}

TextureView* VulkanSwapchain::acquireNextTexture()
{
    VulkanDevice* vulkanDevice = downcast(m_device);
    const VulkanAPI& vkAPI = vulkanDevice->vkAPI;

    VkResult result = vkAPI.AcquireNextImageKHR(vulkanDevice->getVkDevice(), m_swapchain, UINT64_MAX, m_presentSemaphore, VK_NULL_HANDLE, &m_acquiredImageIndex);
    if (result != VK_SUCCESS)
    {
        spdlog::error("Failed to acquire next image index. error: {}", static_cast<int32_t>(result));
        return nullptr;
    }

    return m_textureViews[m_acquiredImageIndex].get();
}

VkSwapchainKHR VulkanSwapchain::getVkSwapchainKHR() const
{
    return m_swapchain;
}

std::pair<VkSemaphore, VkPipelineStageFlags> VulkanSwapchain::getPresentSemaphore() const
{
    return { m_presentSemaphore, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
}

std::pair<VkSemaphore, VkPipelineStageFlags> VulkanSwapchain::getRenderSemaphore() const
{
    return { m_renderSemaphore, VK_PIPELINE_STAGE_NONE };
}

VkCompositeAlphaFlagBitsKHR VulkanSwapchain::getCompositeAlphaFlagBit(VkCompositeAlphaFlagsKHR supportedCompositeAlpha)
{
    std::array<VkCompositeAlphaFlagBitsKHR, 4> compositeAlphaFlagBits = {
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
    };

    for (const auto compositeAlphaFlagBit : compositeAlphaFlagBits)
    {
        if (supportedCompositeAlpha & compositeAlphaFlagBit)
        {
            return compositeAlphaFlagBit;
        }
    }

    throw std::runtime_error(fmt::format("Failed to find supported composite alpha flag bit. [{}]", supportedCompositeAlpha));

    return static_cast<VkCompositeAlphaFlagBitsKHR>(0x00000000);
}

// Convert Helper
ColorSpace ToColorSpace(VkColorSpaceKHR colorSpace)
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
VkColorSpaceKHR ToVkColorSpaceKHR(ColorSpace colorSpace)
{
    switch (colorSpace)
    {
    case ColorSpace::kSRGBNonLinear:
        return VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

    case ColorSpace::kSRGBLinear:
        return VK_COLOR_SPACE_EXTENDED_SRGB_LINEAR_EXT;

    case ColorSpace::kUndefined:
    default:
        spdlog::error("color space is undefined. use srgb non linear mode.");
        return VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    }
}

PresentMode ToPresentMode(VkPresentModeKHR mode)
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
VkPresentModeKHR ToVkPresentModeKHR(PresentMode mode)
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
        spdlog::error("Present Mode is undefined. use immediate mode.");
        return VK_PRESENT_MODE_IMMEDIATE_KHR;
    }
}

} // namespace jipu
