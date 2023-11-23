#pragma once

#include "jipu/gpu/texture.h"
#include "utils/assert.h"
#include "utils/cast.h"
#include "vulkan_api.h"
#include "vulkan_memory.h"
#include "vulkan_texture_view.h"

#include <fmt/format.h>

namespace jipu
{

enum class TextureOwner
{
    External,
    Internal // by swap chain
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
    TextureOwner getTextureOwner() const;

    /// @brief record pipeline barrier command, but not submitted.
    void setLayout(VkCommandBuffer commandBuffer, VkImageLayout layout, VkImageSubresourceRange range);
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
VkFormat ToVkFormat(TextureFormat format);
TextureFormat ToTextureFormat(VkFormat format);
VkImageType ToVkImageType(TextureType type);
TextureType ToTextureType(VkImageType type);
VkImageUsageFlags ToVkImageUsageFlags(TextureUsageFlags usages);
TextureUsageFlags ToTextureUsageFlags(VkImageUsageFlags usages);
VkSampleCountFlagBits ToVkSampleCountFlagBits(uint32_t count);

// Utils
VkImageLayout GenerateImageLayout(TextureUsageFlags usage);

} // namespace jipu
