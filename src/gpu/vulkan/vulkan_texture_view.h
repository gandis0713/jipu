#pragma once

#include "gpu/texture_view.h"

namespace vkt
{

class VulkanTexture;
class VulkanTextureView : public TextureView
{
public:
    VulkanTextureView() = delete;
    VulkanTextureView(VulkanTexture* texture, TextureViewCreateInfo info);
    virtual ~VulkanTextureView() = default;
};

} // namespace vkt
