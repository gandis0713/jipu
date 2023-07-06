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

enum BindingStageFlagBits : uint32_t
{
    // 0x00000000 is undefined,
    kVertexStage = 0x00000001,
    kFragmentStage = 0x00000002,
    kComputeStage = 0x00000004,
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
};

struct TextureBindingLayout : BindingLayout
{
};

struct BindingGroupLayoutDescriptor
{
    std::vector<BufferBindingLayout> buffers = {};
    std::vector<TextureBindingLayout> textures = {};
};

class Device;
class VKT_EXPORT BindingGroupLayout
{
public:
    BindingGroupLayout() = delete;
    BindingGroupLayout(Device* device, const BindingGroupLayoutDescriptor& descriptor);
    virtual ~BindingGroupLayout() = default;

    const std::vector<BufferBindingLayout>& getBufferBindingLayouts() const;
    BufferBindingLayout getBufferBindingLayout(uint32_t index) const;

    const std::vector<TextureBindingLayout>& getTextureBindingLayouts() const;
    TextureBindingLayout getTextureBindingLayout(uint32_t index) const;

protected:
    Device* m_device = nullptr;
    BindingGroupLayoutDescriptor m_descriptor{};
};

} // namespace vkt