#pragma once

#include "jipu/gpu/texture_view.h"
#include "utils/cast.h"
#include "vulkan_api.h"

namespace jipu
{

class VulkanTexture;
class VulkanTextureView : public TextureView
{
public:
    VulkanTextureView() = delete;
    VulkanTextureView(VulkanTexture* texture, TextureViewDescriptor descriptor);
    ~VulkanTextureView() override;

    VkImageView getVkImageView() const;

private:
    VkImageView m_imageView = VK_NULL_HANDLE;
};

DOWN_CAST(VulkanTextureView, TextureView);

// Convert Helper
VkImageViewType ToVkImageViewType(TextureViewType type);
TextureViewType ToTextureViewType(VkImageViewType type);
VkImageAspectFlags ToVkImageAspectFlags(TextureAspectFlags flags);
TextureAspectFlags ToTextureAspectFlags(VkImageAspectFlags flags);

} // namespace jipu