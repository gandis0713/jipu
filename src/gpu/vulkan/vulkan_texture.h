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

inline VkImageType type2Vk(TextureType type)
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

inline TextureType vk2Type(VkImageType type)
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

inline VkFormat format2Vk(TextureFormat format)
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

inline TextureFormat vk2Format(VkFormat format)
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
