#include "jipu/binding_group.h"

#include "jipu/binding_group_layout.h"
#include "jipu/device.h"
#include "jipu/sampler.h"
#include "jipu/texture_view.h"

#include <fmt/format.h>
#include <stdexcept>

namespace jipu
{

BindingGroup::BindingGroup(Device* device, const BindingGroupDescriptor& descriptor)
    : m_device(device)
    , m_descriptor(descriptor)
{
}

const std::vector<BufferBinding>& BindingGroup::getBufferBindings() const
{
    return m_descriptor.buffers;
}

BufferBinding BindingGroup::getBufferBinding(uint32_t index) const
{
    for (const auto& buffer : m_descriptor.buffers)
    {
        if (buffer.index == index)
            return buffer;
    }

    throw std::runtime_error(fmt::format("Failed to find buffer binding. {}", index));
    return {};
}

const std::vector<SamplerBinding>& BindingGroup::getSamplerBindings() const
{
    return m_descriptor.samplers;
}

SamplerBinding BindingGroup::getSamplerBinding(uint32_t index) const
{
    for (const auto& sampler : m_descriptor.samplers)
    {
        if (sampler.index == index)
            return sampler;
    }

    throw std::runtime_error(fmt::format("Failed to find sampler binding. {}", index));
    return {};
}

const std::vector<TextureBinding>& BindingGroup::getTextureBindings() const
{
    return m_descriptor.textures;
}

TextureBinding BindingGroup::getTextureBinding(uint32_t index) const
{
    for (const auto& texture : m_descriptor.textures)
    {
        if (texture.index == index)
            return texture;
    }

    throw std::runtime_error(fmt::format("Failed to find texture binding. {}", index));
    return {};
}

} // namespace jipu