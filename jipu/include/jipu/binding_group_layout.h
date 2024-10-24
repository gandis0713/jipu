#pragma once

#include "export.h"
#include <optional>
#include <vector>

namespace jipu
{

enum class BufferBindingType
{
    kUndefined = 0,
    kUniform,
    kStorage
};

struct BindingStageFlagBits
{
    static constexpr uint32_t kUndefined = 1 << 0;     // 0x00000000
    static constexpr uint32_t kVertexStage = 1 << 1;   // 0x00000001
    static constexpr uint32_t kFragmentStage = 1 << 2; // 0x00000002
    static constexpr uint32_t kComputeStage = 1 << 3;  // 0x00000004
};
using BindingStageFlags = uint32_t;

struct BindingLayout
{
    /// @brief The index of binding.
    uint32_t index = 0;
    BindingStageFlags stages = 0u;
};

struct BufferBindingLayout : BindingLayout
{
    BufferBindingType type = BufferBindingType::kUndefined;
    bool dynamicOffset = false;
};

struct SamplerBindingLayout : BindingLayout
{
};

struct TextureBindingLayout : BindingLayout
{
};

struct BindingGroupLayoutDescriptor
{
    std::vector<BufferBindingLayout> buffers = {};
    std::vector<SamplerBindingLayout> samplers = {};
    std::vector<TextureBindingLayout> textures = {};
};

class Device;
class JIPU_EXPORT BindingGroupLayout
{
public:
    virtual ~BindingGroupLayout() = default;
};

} // namespace jipu