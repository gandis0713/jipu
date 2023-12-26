#pragma once

#include "export.h"
#include <optional>
#include <vector>

namespace jipu
{

struct BindingLayout;
struct Binding
{
    /// @brief The index of binding.
    uint32_t index = 0;
};

class Buffer;
struct BufferBinding : Binding
{
    Buffer* buffer = nullptr;
    uint64_t offset = 0;
    uint64_t size = 0;
};

class Sampler;
class TextureView;
struct SamplerBinding : Binding
{
    Sampler* sampler = nullptr;
    TextureView* textureView = nullptr;
};

struct TextureBinding : Binding
{
    TextureView* textureView = nullptr;
};

class BindingGroupLayout;
struct BindingGroupDescriptor
{
    BindingGroupLayout* layout = nullptr;
    std::vector<BufferBinding> buffers = {};
    std::vector<SamplerBinding> samplers = {};
    std::vector<TextureBinding> textures = {};
};

class Device;
class JIPU_EXPORT BindingGroup
{
public:
    BindingGroup() = delete;
    BindingGroup(Device* device, const BindingGroupDescriptor& descriptor);
    virtual ~BindingGroup() = default;

    const std::vector<BufferBinding>& getBufferBindings() const;
    std::optional<BufferBinding> getBufferBinding(uint32_t index) const;

    const std::vector<SamplerBinding>& getSamplerBindings() const;
    std::optional<SamplerBinding> getSamplerBinding(uint32_t index) const;

    const std::vector<TextureBinding>& getTextureBindings() const;
    std::optional<TextureBinding> getTextureBinding(uint32_t index) const;

protected:
    Device* m_device = nullptr;
    BindingGroupDescriptor m_descriptor{};
};

} // namespace jipu