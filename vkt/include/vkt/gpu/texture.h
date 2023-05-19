#pragma once

#include "export.h"

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
};

struct TextureDescriptor
{
    TextureType type;
    TextureFormat format;
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

    Device* getDevice() const;

protected:
    Device* m_device = nullptr;

    TextureType m_type;
    TextureFormat m_format;
};

} // namespace vkt
