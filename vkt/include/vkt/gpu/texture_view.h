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

    Texture* getTexture() const;

    TextureViewType getType() const;
    uint32_t getWidth() const;
    uint32_t getHeight() const;
    uint32_t getMipLevels() const;
    uint32_t getSampleCount() const;

protected:
    Texture* m_texture = nullptr;
    TextureViewDescriptor m_descriptor{};
};

} // namespace vkt
