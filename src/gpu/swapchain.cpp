#include "gpu/swapchain.h"

#include "gpu/device.h"
#include "gpu/surface.h"

namespace vkt
{

SwapChain::SwapChain(Device* device, SwapChainDescriptor descriptor) noexcept
    : m_device(device)
    , m_surface(std::move(descriptor.surface))
{
}

std::vector<Texture*> SwapChain::getTextures() const
{
    std::vector<Texture*> textures{};

    for (auto& texture : m_textures)
    {
        textures.push_back(texture.get());
    }

    return textures;
}
std::vector<TextureView*> SwapChain::getTextureViews() const
{
    std::vector<TextureView*> textureViews{};

    for (auto& textureView : m_textureViews)
    {
        textureViews.push_back(textureView.get());
    }

    return textureViews;
}

} // namespace vkt
