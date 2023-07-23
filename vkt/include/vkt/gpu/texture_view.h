#pragma once

#include "export.h"
#include <stdint.h>

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

enum class TextureFormat
{
    kUndefined = 0,
    kBGRA_8888_UInt_Norm,
    kBGRA_8888_UInt_Norm_SRGB,
    kRGB_888_UInt_Norm,
    kRGB_888_UInt_Norm_SRGB,
    kRGBA_8888_UInt_Norm,
    kRGBA_8888_UInt_Norm_SRGB
};

struct TextureViewDescriptor
{
    TextureViewType type;
};

class Texture;
class VKT_EXPORT TextureView
{
public:
    TextureView() = delete;
    TextureView(Texture* texture, TextureViewDescriptor descriptor);
    virtual ~TextureView() = default;

    TextureFormat getFormat() const;
    uint32_t getWidth() const;
    uint32_t getHeight() const;

protected:
    Texture* m_texture = nullptr;
};

} // namespace vkt
