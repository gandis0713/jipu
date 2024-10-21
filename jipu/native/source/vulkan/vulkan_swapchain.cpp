#include "vulkan_swapchain.h"

#include "vulkan_device.h"
#include "vulkan_physical_device.h"
#include "vulkan_queue.h"
#include "vulkan_surface.h"

#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include <stdexcept>

namespace jipu
{

namespace
{

VkCompositeAlphaFlagBitsKHR getCompositeAlphaFlagBit(VkCompositeAlphaFlagsKHR supportedCompositeAlpha)
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
} // namespace

VulkanSwapchainDescriptor generateVulkanSwapchainDescriptor(VulkanDevice& device, const SwapchainDescriptor& descriptor)
{
    VulkanSurface* surface = downcast(descriptor.surface);

    auto& vulkanPhysicalDevice = device.getPhysicalDevice();
    VulkanSurfaceInfo surfaceInfo = vulkanPhysicalDevice.gatherSurfaceInfo(surface);

    // Check surface formats supports.
    auto surfaceFormatIter = std::find_if(surfaceInfo.formats.begin(),
                                          surfaceInfo.formats.end(),
                                          [textureFormat = descriptor.textureFormat, colorSpace = descriptor.colorSpace](const VkSurfaceFormatKHR& surfaceFormat) { return surfaceFormat.format == ToVkFormat(textureFormat) &&
                                                                                                                                                                           surfaceFormat.colorSpace == ToVkColorSpaceKHR(colorSpace); });
    if (surfaceFormatIter == surfaceInfo.formats.end())
    {
        throw std::runtime_error(fmt::format("{} texture format or/and {} color space are not supported.",
                                             static_cast<uint32_t>(descriptor.textureFormat),
                                             static_cast<uint32_t>(descriptor.colorSpace)));
    }
    const VkSurfaceFormatKHR surfaceFormat = *surfaceFormatIter;

    // Check surface present mode.
    auto presentModeIter = std::find(surfaceInfo.presentModes.begin(),
                                     surfaceInfo.presentModes.end(),
                                     ToVkPresentModeKHR(descriptor.presentMode));
    if (presentModeIter == surfaceInfo.presentModes.end())
    {
        throw std::runtime_error(fmt::format("{} present mode is not supported.", static_cast<uint32_t>(descriptor.presentMode)));
    }
    const VkPresentModeKHR presentMode = *presentModeIter;

    // Check surface capabilities.
    const VkSurfaceCapabilitiesKHR& surfaceCapabilities = surfaceInfo.capabilities;
    uint32_t imageCount = surfaceCapabilities.minImageCount + 1;
    if (surfaceCapabilities.maxImageCount > 0 && imageCount > surfaceCapabilities.maxImageCount)
    {
        imageCount = surfaceCapabilities.maxImageCount;
    }

    uint32_t width = descriptor.width;
    uint32_t height = descriptor.height;

    // If extent is invalid, use current extent.
    if (descriptor.width < surfaceCapabilities.minImageExtent.width ||
        descriptor.width > surfaceCapabilities.maxImageExtent.width ||
        descriptor.height < surfaceCapabilities.minImageExtent.height ||
        descriptor.height > surfaceCapabilities.maxImageExtent.height)
    {
        width = surfaceCapabilities.currentExtent.width;
        height = surfaceCapabilities.currentExtent.height;
    }

    VulkanSwapchainDescriptor vkdescriptor{
        .surface = surface
    };
    vkdescriptor.minImageCount = imageCount;
    vkdescriptor.imageFormat = surfaceFormat.format;
    vkdescriptor.imageColorSpace = surfaceFormat.colorSpace;
    vkdescriptor.imageExtent = { width, height };
    vkdescriptor.imageArrayLayers = 1;
    vkdescriptor.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // TODO: check sharing mode
    // if graphics and present queue family are difference.
    // {
    //     vkdescriptor.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
    //     vkdescriptor.queueFamilyIndices.resize(2);
    //     vkdescriptor.pQueueFamilyIndices = queueFamilyIndices;
    // }
    // else
    // {
    //     vkdescriptor.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    // }

    vkdescriptor.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    vkdescriptor.queueFamilyIndices = {}; // Optional
    vkdescriptor.preTransform = surfaceCapabilities.currentTransform;
    vkdescriptor.compositeAlpha = getCompositeAlphaFlagBit(surfaceCapabilities.supportedCompositeAlpha);
    vkdescriptor.presentMode = presentMode;
    vkdescriptor.clipped = VK_TRUE;
    vkdescriptor.oldSwapchain = VK_NULL_HANDLE;
    vkdescriptor.queue = downcast(descriptor.queue);

    return vkdescriptor;
}

VulkanSwapchain::VulkanSwapchain(VulkanDevice& device, const SwapchainDescriptor& descriptor) noexcept(false)
    : VulkanSwapchain(device, generateVulkanSwapchainDescriptor(device, descriptor))
{
}

VulkanSwapchain::VulkanSwapchain(VulkanDevice& device, const VulkanSwapchainDescriptor& descriptor) noexcept(false)
    : m_device(device)
    , m_descriptor(descriptor)
{
    VkSwapchainCreateInfoKHR swapchainCreateInfo{};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = descriptor.surface->getVkSurface();
    swapchainCreateInfo.minImageCount = descriptor.minImageCount;
    swapchainCreateInfo.imageFormat = descriptor.imageFormat;
    swapchainCreateInfo.imageColorSpace = descriptor.imageColorSpace;
    swapchainCreateInfo.imageExtent = descriptor.imageExtent;
    swapchainCreateInfo.imageArrayLayers = descriptor.imageArrayLayers;
    swapchainCreateInfo.imageUsage = descriptor.imageUsage;
    swapchainCreateInfo.imageSharingMode = descriptor.imageSharingMode;
    swapchainCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(descriptor.queueFamilyIndices.size());
    swapchainCreateInfo.pQueueFamilyIndices = descriptor.queueFamilyIndices.data();
    swapchainCreateInfo.preTransform = descriptor.preTransform;
    swapchainCreateInfo.compositeAlpha = descriptor.compositeAlpha;
    swapchainCreateInfo.presentMode = descriptor.presentMode;
    swapchainCreateInfo.clipped = descriptor.clipped;
    swapchainCreateInfo.oldSwapchain = descriptor.oldSwapchain;

    const VulkanAPI& vkAPI = m_device.vkAPI;
    if (vkAPI.CreateSwapchainKHR(m_device.getVkDevice(), &swapchainCreateInfo, nullptr, &m_swapchain) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create swap chain");
    }

    // get swapchain image.
    vkAPI.GetSwapchainImagesKHR(m_device.getVkDevice(), m_swapchain, &swapchainCreateInfo.minImageCount, nullptr);

    std::vector<VkImage> images{};
    images.resize(swapchainCreateInfo.minImageCount);
    vkAPI.GetSwapchainImagesKHR(m_device.getVkDevice(), m_swapchain, &swapchainCreateInfo.minImageCount, images.data());

    // create Textures by VkImage.
    for (VkImage image : images)
    {
        VulkanTextureDescriptor descriptor{};
        descriptor.image = image; // by swapchain

        descriptor.imageType = VK_IMAGE_TYPE_2D;
        descriptor.extent = { swapchainCreateInfo.imageExtent.width, swapchainCreateInfo.imageExtent.height, 1 };
        descriptor.mipLevels = 1;
        descriptor.arrayLayers = 1;
        descriptor.format = swapchainCreateInfo.imageFormat;
        descriptor.tiling = VK_IMAGE_TILING_OPTIMAL;
        descriptor.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        descriptor.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
        descriptor.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        descriptor.samples = VK_SAMPLE_COUNT_1_BIT;
        descriptor.flags = 0; // Optional

        std::unique_ptr<VulkanTexture> texture = std::make_unique<VulkanTexture>(m_device, descriptor);
        m_textures.push_back(std::move(texture));
    }

    // create TextureViews
    for (std::unique_ptr<VulkanTexture>& texture : m_textures)
    {
        TextureViewDescriptor descriptor{};
        descriptor.dimension = TextureViewDimension::k2D;
        descriptor.aspect = TextureAspectFlagBits::kColor;
        auto textureView = std::make_unique<VulkanTextureView>(*texture, descriptor);
        m_textureViews.push_back(std::move(textureView));
    }

    // create semaphore
    auto swapchainImageSize = m_textures.size();
    m_acquireImageSemaphores.resize(swapchainImageSize); // only resize. do not create semaphore here.
}

VulkanSwapchain::~VulkanSwapchain()
{
    auto& vulkanDevice = downcast(m_device);
    const VulkanAPI& vkAPI = vulkanDevice.vkAPI;

    for (auto& semaphore : m_acquireImageSemaphores)
    {
        if (semaphore != VK_NULL_HANDLE)
            m_device.getSemaphorePool()->release(semaphore);
    }

    /* do not delete VkImages from swapchain. */

    vkAPI.DestroySwapchainKHR(downcast(m_device).getVkDevice(), m_swapchain, nullptr);
}

TextureFormat VulkanSwapchain::getTextureFormat() const
{
    return ToTextureFormat(m_descriptor.imageFormat);
}

uint32_t VulkanSwapchain::getWidth() const
{
    return m_descriptor.imageExtent.width;
}

uint32_t VulkanSwapchain::getHeight() const
{
    return m_descriptor.imageExtent.height;
}

void VulkanSwapchain::present()
{
    VulkanDevice& vulkanDevice = downcast(m_device);
    auto vulkanQueue = downcast(m_descriptor.queue);
    const VulkanAPI& vkAPI = vulkanDevice.vkAPI;

    VulkanPresentInfo presentInfo{};
    presentInfo.signalSemaphore.push_back(m_acquireImageSemaphores[m_acquiredImageIndex]);
    presentInfo.swapchains = { m_swapchain };
    presentInfo.imageIndices = { m_acquiredImageIndex };

    vulkanQueue->present(presentInfo);
}

TextureView* VulkanSwapchain::acquireNextTextureView()
{
    VulkanDevice& vulkanDevice = downcast(m_device);
    const VulkanAPI& vkAPI = vulkanDevice.vkAPI;

    auto semaphore = m_device.getSemaphorePool()->create();
    uint32_t acquireImageIndex = 0;
    VkResult result = vkAPI.AcquireNextImageKHR(vulkanDevice.getVkDevice(), m_swapchain, UINT64_MAX, semaphore, VK_NULL_HANDLE, &acquireImageIndex);
    if (result != VK_SUCCESS)
    {
        spdlog::error("Failed to acquire next image index. error: {}", static_cast<int32_t>(result));
    }

    setAcquireImageSemaphore(semaphore, acquireImageIndex);
    setAcquireImageIndex(acquireImageIndex);

    return m_textureViews[acquireImageIndex].get();
}

void VulkanSwapchain::setAcquireImageIndex(const uint32_t imageIndex)
{
    m_acquiredImageIndex = imageIndex;
}

uint32_t VulkanSwapchain::getAcquireImageIndex() const
{
    return m_acquiredImageIndex;
}

void VulkanSwapchain::setAcquireImageSemaphore(VkSemaphore semaphore, const uint32_t imageIndex)
{
    if (m_acquireImageSemaphores.size() <= imageIndex)
    {
        throw std::runtime_error("Failed to set acquire image semaphore. Invalid image index.");
    }

    if (m_acquireImageSemaphores[imageIndex] != VK_NULL_HANDLE)
    {
        m_device.getSemaphorePool()->release(m_acquireImageSemaphores[imageIndex]);
    }
    m_acquireImageSemaphores[imageIndex] = semaphore;
}

VkSwapchainKHR VulkanSwapchain::getVkSwapchainKHR() const
{
    return m_swapchain;
}

} // namespace jipu
