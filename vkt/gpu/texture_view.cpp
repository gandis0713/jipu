#include "vkt/gpu/texture_view.h"
#include "vkt/gpu/texture.h"

namespace jipu
{

TextureView::TextureView(Texture* texture, TextureViewDescriptor descriptor)
    : m_texture(texture)
    , m_descriptor(descriptor)
{
}

Texture* TextureView::getTexture() const
{
    return m_texture;
}

TextureViewType TextureView::getType() const
{
    return m_descriptor.type;
}

uint32_t TextureView::getWidth() const
{
    return m_texture->getWidth();
}

uint32_t TextureView::getHeight() const
{
    return m_texture->getHeight();
}

uint32_t TextureView::getMipLevels() const
{
    return m_texture->getMipLevels();
}

uint32_t TextureView::getSampleCount() const
{
    return m_texture->getSampleCount();
}

} // namespace jipu
