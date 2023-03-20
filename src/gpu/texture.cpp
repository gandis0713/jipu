#include "gpu/texture.h"
#include "gpu/device.h"

namespace vkt
{

Texture::Texture(Device* device, TextureCreateInfo info) : m_device(device) {}

} // namespace vkt