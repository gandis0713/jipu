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

/**
 * @brief Render Pipeline
 */

// Input Assembly
enum class PrimitiveTopology
{
    kUndefined = 0,
    kPointList,
    kLineStrip,
    kLineList,
    kTriangleStrip,
    kTriangleList
};

struct InputAssemblyStage
{
    PrimitiveTopology topology = PrimitiveTopology::kUndefined;
};

// Vertex Stage
enum class VertexFormat
{
    kUndefined = 0,
    kR32_SInt,
    kR32G32_SInt,
    kR32G32B32_SInt,
    kR32G32B32A32_SInt,
    kR32_UInt,
    kR32G32_UInt,
    kR32G32B32_UInt,
    kR32G32B32A32_UInt,
    kR32_SFloat,
    kR32G32_SFloat,
    kR32G32B32_SFloat,
    kR32G32B32A32_SFloat,
};

enum class VertexMode
{
    kVertex = 0,
    kInstance,
};

struct VertexAttribute
{
    uint64_t offset = 0u;
    VertexFormat format = VertexFormat::kUndefined;
};

struct VertexStage : ProgrammableStage
{
    struct Layout
    {
        uint64_t stride = 0u;
        VertexMode mode = VertexMode::kVertex;
        std::vector<VertexAttribute> attributes{};
    };

    std::vector<VertexStage::Layout> layouts{};
};

// Rasterization Stage
struct RasterizationStage
{
};

// Fragment Shader Stage
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
    InputAssemblyStage inputAssembly{};
    VertexStage vertex{};
    RasterizationStage rasterization{};
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
