#pragma once

#include "gpu/texture.h"
#include "vulkan_api.h"

#include "utils/assert.h"

#include <fmt/format.h>
namespace vkt
{

enum class TextureOwner
{
    External,
    Internal
};

class VulkanDevice;
class VulkanTexture : public Texture
{
public:
    VulkanTexture() = delete;
    VulkanTexture(VulkanDevice* device, TextureDescriptor descriptor);

    /**
     * @brief Construct a new Vulkan Texture object.
     *        Have not VkImage ownership.
     */
    VulkanTexture(VulkanDevice* device, VkImage image, TextureDescriptor descriptor);
    ~VulkanTexture() override;

    TextureType getType() const override;
    TextureFormat getFormat() const override;

    VkImage getImage() const;

private:
    VkImage m_image{ nullptr };
    VkImageType m_type{};
    VkFormat m_format{ VK_FORMAT_UNDEFINED };

    TextureOwner m_owner;
};

// Convert Helper
inline VkImageType textureType2VkImageType(TextureType type);
inline TextureType vkImageType2TextureType(VkImageType type);
inline VkFormat textureFormat2VkFormat(TextureFormat format);
inline TextureFormat vkFormat2TextureFormat(VkFormat format);

} // namespace vkt
