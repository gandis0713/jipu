#include "vulkan_texture.h"
#include "vulkan_device.h"

namespace vkt
{

VulkanTexture::VulkanTexture(VulkanDevice* device, TextureDescriptor descriptor)
    : Texture(device, descriptor)
    , m_type(textureType2VkImageType(descriptor.type))
    , m_format(textureFormat2VkFormat(descriptor.format))
    , m_owner(TextureOwner::Internal)
{
    // TODO: create VkImage
}

VulkanTexture::VulkanTexture(VulkanDevice* device, VkImage image, TextureDescriptor descriptor)
    : Texture(device, descriptor)
    , m_image(image)
    , m_type(textureType2VkImageType(descriptor.type))
    , m_format(textureFormat2VkFormat(descriptor.format))
    , m_owner(TextureOwner::External)
{
}

VulkanTexture::~VulkanTexture()
{
    if (m_owner == TextureOwner::Internal)
    {
        VulkanDevice* vulkanDevice = static_cast<VulkanDevice*>(m_device);
        vkDestroyImage(vulkanDevice->getDevice(), m_image, nullptr);
    }
}

TextureType VulkanTexture::getType() const
{
    return vkImageType2TextureType(m_type);
}

TextureFormat VulkanTexture::getFormat() const
{
    return vkFormat2TextureFormat(m_format);
}

VkImage VulkanTexture::getImage() const
{
    return m_image;
}

// Convert Helper
VkImageType textureType2VkImageType(TextureType type)
{
    switch (type)
    {
    case TextureType::k1D:
        return VK_IMAGE_TYPE_1D;
    case TextureType::k2D:
        return VK_IMAGE_TYPE_2D;
    case TextureType::k3D:
        return VK_IMAGE_TYPE_3D;
    default:
        assert_message(false, fmt::format("{} type does not support.", static_cast<uint32_t>(type)));
        return VK_IMAGE_TYPE_1D;
    }
}

TextureType vkImageType2TextureType(VkImageType type)
{
    switch (type)
    {
    case VK_IMAGE_TYPE_1D:
        return TextureType::k1D;
    case VK_IMAGE_TYPE_2D:
        return TextureType::k2D;
    case VK_IMAGE_TYPE_3D:
        return TextureType::k3D;
    default:

        assert_message(false, fmt::format("{} type does not support.", static_cast<uint32_t>(type)));
        return TextureType::kUndefined;
    }
}

VkFormat textureFormat2VkFormat(TextureFormat format)
{
    switch (format)
    {
    case TextureFormat::kBGRA_8888_UInt_Norm:
        return VK_FORMAT_B8G8R8A8_SRGB;
    default:

        assert_message(false, fmt::format("{} format does not support.", static_cast<uint32_t>(format)));
        return VK_FORMAT_UNDEFINED;
    }
}

TextureFormat vkFormat2TextureFormat(VkFormat format)
{
    switch (format)
    {
    case VK_FORMAT_B8G8R8A8_SRGB:
        return TextureFormat::kBGRA_8888_UInt_Norm;
    default:
        assert_message(false, fmt::format("{} format does not support.", static_cast<uint32_t>(format)));
        return TextureFormat::kUndefined;
    }
}

} // namespace vkt
