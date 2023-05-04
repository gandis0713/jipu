#pragma once

#include "utils/assert.h"
#include "utils/cast.h"
#include "vkt/gpu/texture.h"
#include "vulkan_api.h"

#include <fmt/format.h>

namespace vkt
{

enum class TextureOwner
{
    External,
    Internal
};

class VulkanDevice;
class VKT_EXPORT VulkanTexture : public Texture
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

    VkImage getVkImage() const;

private:
    VkImage m_image{ VK_NULL_HANDLE };
    VkImageType m_type{};
    VkFormat m_format{ VK_FORMAT_UNDEFINED };

    TextureOwner m_owner;
};

DOWN_CAST(VulkanTexture, Texture);

// Convert Helper
VkImageType TextureType2VkImageType(TextureType type);
TextureType VkImageType2TextureType(VkImageType type);
VkFormat TextureFormat2VkFormat(TextureFormat format);
TextureFormat VkFormat2TextureFormat(VkFormat format);

} // namespace vkt
