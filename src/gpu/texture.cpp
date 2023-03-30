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

} // namespace vkt
