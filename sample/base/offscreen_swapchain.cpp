#include "offscreen_swapchain.h"

#include "jipu/device.h"
#include "jipu/texture.h"
#include "jipu/texture_view.h"

namespace jipu
{

OffscreenSwapchain::OffscreenSwapchain(Device* device, const SwapchainDescriptor& descriptor)
    : Swapchain(device, descriptor)
{
    auto textureSize = 2; // TODO: set from descriptor

    TextureDescriptor textureDescriptor;
    textureDescriptor.width = descriptor.width;
    textureDescriptor.height = descriptor.height;
    textureDescriptor.format = descriptor.textureFormat;
    textureDescriptor.usage = TextureUsageFlagBits::kColorAttachment;
    textureDescriptor.type = TextureType::k2D;
    textureDescriptor.sampleCount = 1; // TODO: set from descriptor
    textureDescriptor.mipLevels = 1;   // TODO: set from descriptor

    TextureViewDescriptor textureViewDescriptor;
    textureViewDescriptor.aspect = TextureAspectFlagBits::kColor;
    textureViewDescriptor.type = TextureViewType::k2D;
    for (auto i = 0; i < textureSize; ++i)
    {
        auto texture = device->createTexture(textureDescriptor);
        auto textureView = texture->createTextureView(textureViewDescriptor);

        m_textures.push_back(std::move(texture));
        m_textureViews.push_back(std::move(textureView));
    }
}

void OffscreenSwapchain::present(Queue* queue)
{
    // do nothing
}

int OffscreenSwapchain::acquireNextTexture()
{
    auto nextTextureIndex = m_currentTextureIndex + 1;
    if (nextTextureIndex < m_textureViews.size())
    {
        m_currentTextureIndex = nextTextureIndex;
    }
    else
    {
        m_currentTextureIndex = 0;
    }

    return m_currentTextureIndex;
}

} // namespace jipu