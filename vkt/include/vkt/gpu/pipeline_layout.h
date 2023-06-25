#pragma once

#include "export.h"

#include <stdint.h>
#include <vector>

namespace vkt
{

class Device;

enum class BindingType
{
    kUndefined = 0,
    kUniformBuffer,
    kStorageBuffer
};

enum BindingStageBits : uint32_t
{
    // 0x00000000 is undefined,
    kVertexStage = 0x00000001,
    kFragmentStage = 0x00000002,
    kComputeStage = 0x00000004,
};
using BindingStages = uint32_t;

struct BindingResourceDescriptor
{
    /// @brief The index of binding.
    uint32_t binding = 0;
    BindingType type = BindingType::kUndefined;
    BindingStages stages = 0u;
};

class VKT_EXPORT BindingResource
{
public:
    BindingResource() = delete;
    BindingResource(Device* device, const BindingResourceDescriptor& descriptor);
    virtual ~BindingResource() = default;

protected:
    Device* m_device = nullptr;
};

struct BindingLayoutDescriptor
{
    std::vector<BindingResource> bindings = {};
};

class VKT_EXPORT BindingLayout
{
public:
    BindingLayout() = delete;
    BindingLayout(Device* device, const BindingLayoutDescriptor& descriptor);
    virtual ~BindingLayout() = default;

protected:
    Device* m_device = nullptr;
};

struct PipelineLayoutDescriptor
{
    std::vector<BindingLayout> layouts = {};
};

class VKT_EXPORT PipelineLayout
{
public:
    PipelineLayout() = delete;
    PipelineLayout(Device* device, const PipelineLayoutDescriptor& descriptor);
    virtual ~PipelineLayout() = default;

protected:
    Device* m_device = nullptr;
};

} // namespace vkt