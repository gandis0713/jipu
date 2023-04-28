#include "gpu/swapchain.h"

#include "gpu/device.h"
#include "gpu/surface.h"

namespace vkt
{

SwapChain::SwapChain(Device* device, const SwapChainDescriptor& descriptor) noexcept
    : m_device(device)
    , m_textureFormat(descriptor.textureFormat)
    , m_presentMode(descriptor.presentMode)
    , m_width(descriptor.width)
    , m_height(descriptor.height)
    , m_surface(descriptor.surface)
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

TextureFormat SwapChain::getTextureFormat() const
{
    return m_textureFormat;
}

PresentMode SwapChain::getPresentMode() const
{
    return m_presentMode;
}

uint32_t SwapChain::getWidth() const
{
    return m_width;
}

uint32_t SwapChain::getHeight() const
{
    return m_height;
}

} // namespace vkt
