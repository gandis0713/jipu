#include "gpu/texture_view.h"
#include "gpu/texture.h"

namespace vkt
{

TextureView::TextureView(Texture* texture, TextureViewDescriptor descriptor)
    : m_texture(texture)
{
}

} // namespace vkt
