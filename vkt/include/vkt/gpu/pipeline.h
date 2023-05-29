#pragma once

#include "export.h"
#include "shader_module.h"
#include "texture.h"

#include <string>
#include <vector>

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

struct ProgrammableStage
{
    ShaderModule* shader = nullptr;
    std::string entryPoint = "main";
};

// Render Pipeline
struct VertexStage : ProgrammableStage
{
};

struct FragmentStage : ProgrammableStage
{
    struct Target
    {
        TextureFormat format;
    };

    std::vector<FragmentStage::Target> targets{};
};

struct RenderPipelineDescriptor
{
    VertexStage vertex{};
    FragmentStage fragment{};
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
    const RenderPipelineDescriptor m_descriptor{};
};

} // namespace vkt
