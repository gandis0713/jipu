#pragma once

#include "export.h"
#include <vector>

namespace vkt
{

enum class BufferBindingType
{
    kUndefined = 0,
    kUniform,
    kStorage
};

struct BufferBindingLayout
{
    BufferBindingType type = BufferBindingType::kUndefined;
};

struct TextureBindingLayout
{
};

enum BindingStageFlagBits : uint32_t
{
    // 0x00000000 is undefined,
    kVertexStage = 0x00000001,
    kFragmentStage = 0x00000002,
    kComputeStage = 0x00000004,
};
using BindingStageFlags = uint32_t;

struct BindingGroupLayoutEntry
{
    /// @brief The index of binding.
    uint32_t index = 0;
    BindingStageFlags stages = 0u;
};

struct BufferBindingGroupLayoutEntry : BindingGroupLayoutEntry
{
    BufferBindingLayout layout{};
};

struct TextureBindingGroupLayoutEntry : BindingGroupLayoutEntry
{
    TextureBindingLayout layout;
};

struct BindingGroupLayoutDescriptor
{
    std::vector<BufferBindingGroupLayoutEntry> buffers = {};
    std::vector<TextureBindingGroupLayoutEntry> textures = {};
};

class Device;
class VKT_EXPORT BindingGroupLayout
{
public:
    BindingGroupLayout() = delete;
    BindingGroupLayout(Device* device, const BindingGroupLayoutDescriptor& descriptor);
    virtual ~BindingGroupLayout() = default;

protected:
    Device* m_device = nullptr;
};

} // namespace vkt