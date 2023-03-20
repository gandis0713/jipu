#pragma once

#include "gpu/texture.h"

namespace vkt
{

class VulkanDevice;
class VulkanTexture : public Texture
{
public:
    VulkanTexture() = delete;
    VulkanTexture(VulkanDevice* device, TextureCreateInfo info);
    ~VulkanTexture() override = default;
};

} // namespace vkt