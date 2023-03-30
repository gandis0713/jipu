#pragma once

#include "gpu/texture.h"
#include "vulkan_api.h"

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

    VkImage getImage() const;

private:
    VkImage m_image{ nullptr };
    TextureOwner m_owner;
};

} // namespace vkt
