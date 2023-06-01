#pragma once

#include "export.h"
#include <stdint.h>

namespace vkt
{

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
    kRGBA_8888_UInt_Norm
};

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
