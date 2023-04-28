#pragma once

#include "gpu/texture_view.h"
#include "utils/cast.h"
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

    VkImageView getImageView() const;

private:
    VkImageView m_imageView{ VK_NULL_HANDLE };
};

VULKAN_DOWNCAST(TextureView);

} // namespace vkt
