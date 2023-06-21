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
    imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D; // TODO: from texture
    imageViewCreateInfo.format = ToVkFormat(texture->getFormat());

    imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
    imageViewCreateInfo.subresourceRange.levelCount = 1;
    imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
    imageViewCreateInfo.subresourceRange.layerCount = 1;

    auto vulkanDevice = downcast(m_texture->getDevice());
    if (vulkanDevice->vkAPI.CreateImageView(vulkanDevice->getVkDevice(), &imageViewCreateInfo, nullptr, &m_imageView) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create image views!");
    }
}

VulkanTextureView::~VulkanTextureView()
{
    auto vulkanDevice = downcast(m_texture->getDevice());
    vulkanDevice->vkAPI.DestroyImageView(vulkanDevice->getVkDevice(), m_imageView, nullptr);
}

VkImageView VulkanTextureView::getVkImageView() const
{
    return m_imageView;
}

} // namespace vkt
