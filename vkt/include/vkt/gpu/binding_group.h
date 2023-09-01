#pragma once

#include "export.h"
#include <vector>

namespace vkt
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
    // TODO: texture binding
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
class VKT_EXPORT BindingGroup
{
public:
    BindingGroup() = delete;
    BindingGroup(Device* device, const BindingGroupDescriptor& descriptor);
    virtual ~BindingGroup() = default;

    const std::vector<BufferBindingLayout>& getBufferBindingLayouts() const;
    BufferBindingLayout getBufferBindingLayout(uint32_t index) const;

    const std::vector<SamplerBindingLayout>& getSamplerBindingLayouts() const;
    SamplerBindingLayout getSamplerBindingLayout(uint32_t index) const;

    const std::vector<TextureBindingLayout>& getTextureBindingLayouts() const;
    TextureBindingLayout getTextureBindingLayout(uint32_t index) const;

protected:
    Device* m_device = nullptr;
    BindingGroupDescriptor m_descriptor{};
};

} // namespace vkt