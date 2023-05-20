#pragma once

#include "export.h"
#include "shader_module.h"

namespace vkt
{

class Device;

class VKT_EXPORT Pipeline
{
public:
    Pipeline() = delete;
    Pipeline(Device* device);
    virtual ~Pipeline() = default;

    Pipeline(const Pipeline&) = delete;
    Pipeline& operator=(const Pipeline&) = delete;

protected:
    Device* m_device = nullptr;
};

struct RenderPipelineDescriptor
{
    ShaderModule* vertex = nullptr;
    ShaderModule* fragment = nullptr;
};

class VKT_EXPORT RenderPipeline : public Pipeline
{
public:
    RenderPipeline() = delete;
    RenderPipeline(Device* device, const RenderPipelineDescriptor& descriptor);
    virtual ~RenderPipeline() = default;

    RenderPipeline(const RenderPipeline&) = delete;
    RenderPipeline& operator=(const RenderPipeline&) = delete;

protected:
    ShaderModule* m_vertex = nullptr;
    ShaderModule* m_fragment = nullptr;
};

} // namespace vkt
