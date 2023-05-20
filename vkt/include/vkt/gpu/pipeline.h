#pragma once

#include "export.h"
#include "shader_module.h"

namespace vkt
{

class Device;

struct PipelineDescriptor
{
    ShaderModule* vertex = nullptr;
    ShaderModule* fragment = nullptr;
};

class VKT_EXPORT Pipeline
{
public:
    Pipeline() = delete;
    Pipeline(Device* device, const PipelineDescriptor& descriptor);
    virtual ~Pipeline() = default;

    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

protected:
    Device* m_device = nullptr;

    ShaderModule* m_vertex = nullptr;
    ShaderModule* m_fragment = nullptr;
};

} // namespace vkt
