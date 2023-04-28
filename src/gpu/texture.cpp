#include "gpu/texture.h"
#include "gpu/device.h"

namespace vkt
{

Texture::Texture(Device* device, TextureDescriptor descriptor)
    : m_device(device)
{
}

Device* Texture::getDevice() const
{
    return m_device;
}

TextureType Texture::getType() const
{
    return m_type;
}

TextureFormat Texture::getFormat() const
{
    return m_format;
}

} // namespace vkt
