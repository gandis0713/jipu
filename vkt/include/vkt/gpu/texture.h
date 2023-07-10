#pragma once

#include "export.h"
#include <stdint.h>

namespace vkt
{

struct Extent2D
{
    uint32_t width = 0;
    uint32_t height = 0;
};

struct Extent3D : Extent2D
{
    uint32_t depth = 0;
};

enum class TextureType
{
    kUndefined = 0,
    k1D,
    k2D,
    k3D,
};

enum class TextureFormat
{
    kUndefined = 0,
    kBGRA_8888_UInt_Norm,
    kBGRA_8888_UInt_Norm_SRGB,
    kRGBA_8888_UInt_Norm,
    kRGBA_8888_UInt_Norm_SRGB
};

struct TextureUsageFlagBits
{
    static constexpr uint32_t kUndefined = 0x00000000;
    static constexpr uint32_t kCopySrc = 0x00000001;
    static constexpr uint32_t kCopyDst = 0x00000002;
    static constexpr uint32_t kTextureBinding = 0x00000004;
    static constexpr uint32_t kStorageBinding = 0x00000008;
    static constexpr uint32_t kDepthStencil = 0x00000010;
    static constexpr uint32_t kColorAttachment = 0x00000020;
};
using TextureUsageFlags = uint32_t;

struct TextureDescriptor
{
    TextureType type;
    TextureFormat format;
    uint32_t width;
    uint32_t height;
};

class Device;
class VKT_EXPORT Texture
{
public:
    Texture() = delete;
    Texture(Device* device, TextureDescriptor descriptor);
    virtual ~Texture() = default;

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    TextureType getType() const;
    TextureFormat getFormat() const;
    uint32_t getWidth() const;
    uint32_t getHeight() const;

    Device* getDevice() const;

protected:
    Device* m_device = nullptr;

    TextureType m_type;
    TextureFormat m_format;
    uint32_t m_width;
    uint32_t m_height;
};

} // namespace vkt
