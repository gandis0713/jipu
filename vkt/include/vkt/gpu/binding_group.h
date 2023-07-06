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

struct TextureBinding : Binding
{
    // TODO: texture binding
};

class BindingGroupLayout;
struct BindingGroupDescriptor
{
    BindingGroupLayout* layout = nullptr;
    std::vector<BufferBinding> buffers = {};
    std::vector<TextureBinding> textures = {};
};

class Device;
class VKT_EXPORT BindingGroup
{
public:
    BindingGroup() = delete;
    BindingGroup(Device* device, const BindingGroupDescriptor& descriptor);
    virtual ~BindingGroup() = default;

protected:
    Device* m_device = nullptr;
};

} // namespace vkt