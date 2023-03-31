#pragma once

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
class Texture
{
public:
    Texture() = delete;
    Texture(Device* device, TextureDescriptor descriptor);
    virtual ~Texture() = default;

    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;

    virtual TextureType getType() const = 0;
    virtual TextureFormat getFormat() const = 0;

    Device* getDevice() const;

protected:
    Device* m_device{ nullptr };
};

} // namespace vkt
