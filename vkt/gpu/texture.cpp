#include "vkt/gpu/texture.h"
#include "vkt/gpu/device.h"

namespace vkt
{

Texture::Texture(Device* device, TextureDescriptor descriptor)
    : m_device(device)
    , m_type(descriptor.type)
    , m_format(descriptor.format)
    , m_width(descriptor.width)
    , m_height(descriptor.height)
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

uint32_t Texture::getWidth() const
{
    return m_width;
}
uint32_t Texture::getHeight() const
{
    return m_height;
}

} // namespace vkt
