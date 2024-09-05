#pragma once

#include "export.h"
#include <optional>
#include <vector>

namespace jipu
{

class Buffer;
struct BufferBinding
{
    /// @brief The index of binding.
    uint32_t index = 0;
    uint64_t offset = 0;
    uint64_t size = 0;
    Buffer* buffer = nullptr;
};

class Sampler;
class TextureView;
struct SamplerBinding
{
    /// @brief The index of binding.
    uint32_t index = 0;
    Sampler* sampler = nullptr;
};

struct TextureBinding
{
    /// @brief The index of binding.
    uint32_t index = 0;
    TextureView* textureView = nullptr;
};

class BindingGroupLayout;
struct BindingGroupDescriptor
{
    BindingGroupLayout* layout = nullptr;
    std::vector<BufferBinding> buffers{};
    std::vector<SamplerBinding> samplers{};
    std::vector<TextureBinding> textures{};
};

class Device;
class JIPU_EXPORT BindingGroup
{
public:
    virtual ~BindingGroup() = default;

protected:
    BindingGroup() = default;
};

} // namespace jipu