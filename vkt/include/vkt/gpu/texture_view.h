#pragma once

#include "export.h"

namespace vkt
{

enum class TextureViewType
{
    kUndefined = 0,
    k1D,
    k1DArray,
    k2D,
    k2DArray,
    k3D,
    kCube,
    kCubeArray,
};

struct TextureViewDescriptor
{
};

class Texture;
class VKT_EXPORT TextureView
{
public:
    TextureView() = delete;
    TextureView(Texture* texture, TextureViewDescriptor descriptor);
    virtual ~TextureView() = default;

protected:
    Texture* m_texture = nullptr;
};

} // namespace vkt
