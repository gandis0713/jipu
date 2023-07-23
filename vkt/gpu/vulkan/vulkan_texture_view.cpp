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
    imageViewCreateInfo.viewType = ToVkImageViewType(descriptor.type);
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

// Convert Helper

VkFormat ToVkFormat(TextureFormat format)
{
    switch (format)
    {
    case TextureFormat::kBGRA_8888_UInt_Norm:
        return VK_FORMAT_B8G8R8A8_UNORM;
    case TextureFormat::kBGRA_8888_UInt_Norm_SRGB:
        return VK_FORMAT_B8G8R8A8_SRGB;
    case TextureFormat::kRGB_888_UInt_Norm:
        return VK_FORMAT_R8G8B8_UNORM;
    case TextureFormat::kRGB_888_UInt_Norm_SRGB:
        return VK_FORMAT_R8G8B8_SRGB;
    case TextureFormat::kRGBA_8888_UInt_Norm:
        return VK_FORMAT_R8G8B8A8_UNORM;
    case TextureFormat::kRGBA_8888_UInt_Norm_SRGB:
        return VK_FORMAT_R8G8B8A8_SRGB;
    default:
        assert_message(false, fmt::format("{} format does not support.", static_cast<uint32_t>(format)));
        return VK_FORMAT_UNDEFINED;
    }
}

TextureFormat ToTextureFormat(VkFormat format)
{
    switch (format)
    {
    case VK_FORMAT_B8G8R8A8_UNORM:
        return TextureFormat::kBGRA_8888_UInt_Norm;
    case VK_FORMAT_B8G8R8A8_SRGB:
        return TextureFormat::kBGRA_8888_UInt_Norm_SRGB;
    case VK_FORMAT_R8G8B8_UNORM:
        return TextureFormat::kRGB_888_UInt_Norm;
    case VK_FORMAT_R8G8B8_SRGB:
        return TextureFormat::kRGB_888_UInt_Norm_SRGB;
    case VK_FORMAT_R8G8B8A8_UNORM:
        return TextureFormat::kRGBA_8888_UInt_Norm;
    case VK_FORMAT_R8G8B8A8_SRGB:
        return TextureFormat::kRGBA_8888_UInt_Norm_SRGB;
    default:
        assert_message(false, fmt::format("{} format does not support.", static_cast<uint32_t>(format)));
        return TextureFormat::kUndefined;
    }
}

VkImageViewType ToVkImageViewType(TextureViewType type)
{
    switch (type)
    {
    case TextureViewType::k1D:
        return VK_IMAGE_VIEW_TYPE_1D;
    case TextureViewType::k1DArray:
        return VK_IMAGE_VIEW_TYPE_1D_ARRAY;
    case TextureViewType::k2D:
        return VK_IMAGE_VIEW_TYPE_2D;
    case TextureViewType::k2DArray:
        return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
    case TextureViewType::k3D:
        return VK_IMAGE_VIEW_TYPE_3D;
    case TextureViewType::kCube:
        return VK_IMAGE_VIEW_TYPE_CUBE;
    case TextureViewType::kCubeArray:
        return VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
    default:
        assert_message(false, fmt::format("{} type does not support.", static_cast<uint32_t>(type)));
        return VK_IMAGE_VIEW_TYPE_1D;
    }
}
TextureViewType ToTextureViewType(VkImageViewType type)
{
    switch (type)
    {
    case VK_IMAGE_VIEW_TYPE_1D:
        return TextureViewType::k1D;
    case VK_IMAGE_VIEW_TYPE_1D_ARRAY:
        return TextureViewType::k1DArray;
    case VK_IMAGE_VIEW_TYPE_2D:
        return TextureViewType::k2D;
    case VK_IMAGE_VIEW_TYPE_2D_ARRAY:
        return TextureViewType::k2DArray;
    case VK_IMAGE_VIEW_TYPE_3D:
        return TextureViewType::k3D;
    case VK_IMAGE_VIEW_TYPE_CUBE:
        return TextureViewType::kCube;
    case VK_IMAGE_VIEW_TYPE_CUBE_ARRAY:
        return TextureViewType::kCubeArray;
    default:

        assert_message(false, fmt::format("{} type does not support.", static_cast<uint32_t>(type)));
        return TextureViewType::kUndefined;
    }
}

} // namespace vkt
