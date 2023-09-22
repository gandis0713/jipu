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
    kRGBA_8888_UInt_Norm_SRGB,
    kD_32_SFloat,
    kD_24_UInt_Norm_S_8_UInt,
};

struct TextureAspectFlagBits
{
    static constexpr uint32_t kUndefined = 0x00000000;
    static constexpr uint32_t kColor = 0x00000001;
    static constexpr uint32_t kDepth = 0x00000002;
    static constexpr uint32_t kStencil = 0x00000004;
};
using TextureAspectFlags = uint32_t;

struct TextureViewDescriptor
{
    TextureViewType type = TextureViewType::kUndefined;
    TextureAspectFlags aspect = TextureAspectFlagBits::kUndefined;
};

class Texture;
class VKT_EXPORT TextureView
{
public:
    TextureView() = delete;
    TextureView(Texture* texture, TextureViewDescriptor descriptor);
    virtual ~TextureView() = default;

    TextureViewType getType() const;
    TextureFormat getFormat() const;
    uint32_t getWidth() const;
    uint32_t getHeight() const;
    uint32_t getMipLevels() const;
    uint32_t getSampleCount() const;

protected:
    Texture* m_texture = nullptr;
    TextureViewDescriptor m_descriptor{};
};

} // namespace vkt
