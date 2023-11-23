#include "jipu/gpu/binding_group_layout.h"

#include "jipu/gpu/device.h"
#include <fmt/format.h>
#include <stdexcept>

namespace jipu
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

const std::vector<SamplerBindingLayout>& BindingGroupLayout::getSamplerBindingLayouts() const
{
    return m_descriptor.samplers;
}

SamplerBindingLayout BindingGroupLayout::getSamplerBindingLayout(uint32_t index) const
{
    for (const auto& sampler : m_descriptor.samplers)
    {
        if (sampler.index == index)
            return sampler;
    }

    throw std::runtime_error(fmt::format("Failed to find sampler binding layout. {}", index));
    return {};
}

const std::vector<TextureBindingLayout>& BindingGroupLayout::getTextureBindingLayouts() const
{
    return m_descriptor.textures;
}

TextureBindingLayout BindingGroupLayout::getTextureBindingLayout(uint32_t index) const
{
    for (const auto& texture : m_descriptor.textures)
    {
        if (texture.index == index)
            return texture;
    }

    throw std::runtime_error(fmt::format("Failed to find texture binding layout. {}", index));
    return {};
}

} // namespace jipu