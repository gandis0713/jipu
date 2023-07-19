#pragma once

#include "utils/assert.h"
#include "utils/cast.h"
#include "vkt/gpu/texture.h"
#include "vulkan_api.h"
#include "vulkan_memory.h"
#include "vulkan_texture_view.h"

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

    std::unique_ptr<TextureView> createTextureView(const TextureViewDescriptor& descriptor) override;

public:
    VkImage getVkImage() const;

    void setLayout(VkImageLayout layout);
    VkImageLayout getLayout() const;

private:
    VkImage m_image = VK_NULL_HANDLE;
    VkImageType m_type{};
    VkFormat m_format{ VK_FORMAT_UNDEFINED };
    VkImageLayout m_layout = VK_IMAGE_LAYOUT_UNDEFINED;

    std::unique_ptr<VulkanMemory> m_memory = nullptr;
    TextureOwner m_owner;
};

DOWN_CAST(VulkanTexture, Texture);

// Convert Helper
VkImageType ToVkImageType(TextureType type);
TextureType ToTextureType(VkImageType type);
TextureUsageFlags ToTextureUsageFlags(VkImageUsageFlags usages);
VkImageUsageFlags ToVkImageUsageFlags(TextureUsageFlags usages);

} // namespace vkt
