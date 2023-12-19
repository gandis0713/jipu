#include "jipu/binding_group_layout.h"

#include "jipu/device.h"
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

std::optional<BufferBindingLayout> BindingGroupLayout::getBufferBindingLayout(uint32_t index) const
{
    for (const auto& buffer : m_descriptor.buffers)
    {
        if (buffer.index == index)
            return buffer;
    }
    return std::nullopt;
}

const std::vector<SamplerBindingLayout>& BindingGroupLayout::getSamplerBindingLayouts() const
{
    return m_descriptor.samplers;
}

std::optional<SamplerBindingLayout> BindingGroupLayout::getSamplerBindingLayout(uint32_t index) const
{
    for (const auto& sampler : m_descriptor.samplers)
    {
        if (sampler.index == index)
            return sampler;
    }
    return std::nullopt;
}

const std::vector<TextureBindingLayout>& BindingGroupLayout::getTextureBindingLayouts() const
{
    return m_descriptor.textures;
}

std::optional<TextureBindingLayout> BindingGroupLayout::getTextureBindingLayout(uint32_t index) const
{
    for (const auto& texture : m_descriptor.textures)
    {
        if (texture.index == index)
            return texture;
    }
    return std::nullopt;
}

} // namespace jipu