#include "gpu/texture_view.h"
#include "gpu/texture.h"

namespace vkt
{

TextureView::TextureView(Texture* texture, TextureViewCreateInfo info)
    : m_texture(texture)
{
}

} // namespace vkt