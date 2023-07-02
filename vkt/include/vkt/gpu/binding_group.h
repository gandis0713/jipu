#pragma once

#include "export.h"
#include <vector>

namespace vkt
{

struct BindingGroupEntry
{
    /// @brief The index of binding.
    uint32_t index = 0;
};

class Buffer;
struct BufferBinding
{
    Buffer* buffer = nullptr;
    uint64_t offset = 0;
    uint64_t size = 0;
};

struct BufferBindingGroupEntry : BindingGroupEntry
{
    BufferBinding binding;
};

class BindingGroupLayout;
struct BindingGroupDescriptor
{
    BindingGroupLayout* layout = nullptr;
    std::vector<BufferBindingGroupEntry> buffers = {};
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