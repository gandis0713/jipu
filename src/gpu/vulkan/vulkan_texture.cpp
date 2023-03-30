#include "vulkan_texture.h"
#include "vulkan_device.h"

namespace vkt
{

VulkanTexture::VulkanTexture(VulkanDevice* device, TextureCreateInfo info)
    : Texture(device, info)
    , m_owner(TextureOwner::Internal)
{
    // TODO: create VkImage
}

VulkanTexture::VulkanTexture(VulkanDevice* device, VkImage image, TextureCreateInfo info)
    : Texture(device, info)
    , m_image(image)
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

VkImage VulkanTexture::getImage() const
{
    return m_image;
}

} // namespace vkt