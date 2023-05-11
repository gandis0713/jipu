#include "vulkan_texture_view.h"
#include "vulkan_device.h"
#include "vulkan_texture.h"

#include <stdexcept>

namespace vkt
{

VulkanTextureView::VulkanTextureView(VulkanTexture* texture, TextureViewDescriptor descriptor)
    : TextureView(texture, descriptor)
{
    VkImageViewCreateInfo imageViewCreateInfo{};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewCreateInfo.image = texture->getVkImage();
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewCreateInfo.format = VK_FORMAT_B8G8R8A8_SRGB; // TODO: set by texture or surface.

    imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;

    VkDevice device = downcast(m_texture->getDevice())->getVkDevice();
    const VulkanAPI& vkAPI = downcast(m_texture->getDevice())->vkAPI;
    if (vkAPI.CreateImageView(device, &imageViewCreateInfo, nullptr, &m_imageView) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create image views!");
    }
}

VulkanTextureView::~VulkanTextureView()
{
    VkDevice device = downcast(m_texture->getDevice())->getVkDevice();
    const VulkanAPI& vkAPI = downcast(m_texture->getDevice())->vkAPI;

    vkAPI.DestroyImageView(device, m_imageView, nullptr);
}

VkImageView VulkanTextureView::getImageView() const
{
    return m_imageView;
}

} // namespace vkt
