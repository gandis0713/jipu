#include "vkt/gpu/texture_view.h"
#include "vkt/gpu/texture.h"

namespace vkt
{

TextureView::TextureView(Texture* texture, TextureViewDescriptor descriptor)
    : m_texture(texture)
{
}

TextureFormat TextureView::getFormat() const
{
    return m_texture->getFormat();
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

} // namespace vkt
