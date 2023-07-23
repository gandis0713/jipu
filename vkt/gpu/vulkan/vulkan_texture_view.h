#pragma once

#include "utils/cast.h"
#include "vkt/gpu/texture_view.h"
#include "vulkan_api.h"

namespace vkt
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
VkFormat ToVkFormat(TextureFormat format);
TextureFormat ToTextureFormat(VkFormat format);
VkImageViewType ToVkImageViewType(TextureViewType type);
TextureViewType ToTextureViewType(VkImageViewType type);

} // namespace vkt
