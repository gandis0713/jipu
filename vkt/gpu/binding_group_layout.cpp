#include "vkt/gpu/binding_group_layout.h"

#include "vkt/gpu/device.h"
#include <fmt/format.h>
#include <stdexcept>

namespace vkt
{

BindingGroupLayout::BindingGroupLayout(Device* device, const BindingGroupLayoutDescriptor& descriptor)
    : m_device(device)
    , m_descriptor(descriptor)
{
}

const std::vector<BufferBindingLayout>& BindingGroupLayout::getBufferBindingLayouts() const
{
    return m_descriptor.buffers;
}

BufferBindingLayout BindingGroupLayout::getBufferBindingLayout(uint32_t index) const
{
    for (const auto& buffer : m_descriptor.buffers)
    {
        if (buffer.index == index)
            return buffer;
    }

    throw std::runtime_error(fmt::format("Failed to find buffer binding layout. {}", index));

    return {};
}

const std::vector<TextureBindingLayout>& BindingGroupLayout::getTextureBindingLayouts() const
{
    return m_descriptor.textures;
}

} // namespace vkt