#include "vulkan_texture.h"
#include "vulkan_device.h"

namespace vkt
{

VulkanTexture::VulkanTexture(VulkanDevice* device, TextureDescriptor descriptor)
    : Texture(device, descriptor)
    , m_type(type2Vk(descriptor.type))
    , m_format(format2Vk(descriptor.format))
    , m_owner(TextureOwner::Internal)
{
    // TODO: create VkImage
}

VulkanTexture::VulkanTexture(VulkanDevice* device, VkImage image, TextureDescriptor descriptor)
    : Texture(device, descriptor)
    , m_image(image)
    , m_type(type2Vk(descriptor.type))
    , m_format(format2Vk(descriptor.format))
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
    return vk2Type(m_type);
}

TextureFormat VulkanTexture::getFormat() const
{
    return vk2Format(m_format);
}

VkImage VulkanTexture::getImage() const
{
    return m_image;
}

} // namespace vkt
